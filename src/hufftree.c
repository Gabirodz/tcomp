/// @file hufftree.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hufftree.h"
#include "charfreqnode.h"

/**
 * @brief Get the frequency of a node in a HuffTree structure.
 * 
 * @param c 
 * @param t 
 * @return unsigned long 
 */
unsigned long huffTree_get_freq(char c, HuffTree t)
{
    for (struct CharFreqNode *n = t.begin; n->next; n = n->next)
    {
        if (n->c == c)
            return n->val;
    }

    return 0;
}

/**
 * @brief Enqueues a node in the HuffTree structure
 * 
 * Enqueued nodes in the tree will appear at the last horizontal
 * node from the tree parent.
 * 
 * @param node 
 * @param t 
 */
void huffTree_enqueue(struct CharFreqNode *node, HuffTree *t)
{
    if (t->begin == NULL)
    {
        t->begin = node;
        return;
    }

    for (struct CharFreqNode *n = t->begin; n; n = n->next)
    {
        if (!n->next)
        {
            n->next = node;
            return;
        }
    }
}

/**
 * @brief Insert a node horizontally in the correct position of a frequency-ordered tree 
 * 
 * The tree MUST BE ORDERED incrementally by frequency and horizontally,
 * @param node 
 * @param t 
 */
void huffTree_instert_ordered_node(struct CharFreqNode *node, HuffTree *t)
{
    if (!t->begin)
    {
        t->begin = node;
        return;
    }


    for (struct CharFreqNode *n = t->begin; n; n = n->next)
    {
        if ((n->val < node->val) && (!n->next || (n->next->val > node->val)))
        {
            struct CharFreqNode *next_node = n->next;
            n->next = node;
            node->next = next_node;
            break;
        }
        if(!n->next)
        {
            n->next  = node;
            return;
        }
    }
}

/**
 * @brief Dequeues the parent node of the tree and returns it.
 * 
 * The next node (horizontal) will take the begin field of the tree.
 * 
 * @param t 
 * @return struct CharFreqNode* 
 */
struct CharFreqNode *huffTree_dequeue(HuffTree *t)
{
    if (!t->begin)
        return NULL;

    struct CharFreqNode *first_n = t->begin;
    t->begin = t->begin->next;

    return first_n;
}

/**
 * @brief Constructs the Huffman tree
 * 
 * Iteratively take (dequeue) the first two horizontal nodes n1, n2 of the
 * tree, creates a parent node p and assigns the two nodes n1, n2 as its
 * left and right childs, it then enqueues the parent node to the Hufftree.
 * 
 * The process is repeated until the huffTree only has a single node, parent
 * of the rest of the original nodes of the tree, forming the Huffman tree.
 * @param t 
 */
void huffTree_construct_tree(HuffTree *t)
{
    if (t->begin == NULL)
        return;

    int i = 0;
    while (huffTree_length(*t) != 1)
    {
        struct CharFreqNode *new_parent = construct_CharFreqNode(i, 0, NULL);
        new_parent->nextl = huffTree_dequeue(t);
        new_parent->nextr = huffTree_dequeue(t);
        new_parent->val = new_parent->nextl->val + new_parent->nextr->val;

        huffTree_instert_ordered_node(new_parent, t);
        // huffTree_enqueue(new_parent, t);
        ++i;
    }
}
/**
 * @brief Iteratively gets the number of horizontal nodes in a tree.
 * 
 * @param t 
 * @return int 
 */
int huffTree_length(HuffTree t)
{
    if (t.begin == NULL)
        return 0;

    int count = 0;
    for (struct CharFreqNode *n = t.begin; n; n = n->next)
        ++count;

    return count;
}
/**
 * @brief Increases the frequency of a character in the 
 * horizontal nodes of the tree.
 * 
 * @param c 
 * @param t 
 */
void huffTree_increase_freq(char c, HuffTree *t)
{
    struct CharFreqNode *n = t->begin;

    if (!n)
    {
        n = construct_CharFreqNode(c, 1, NULL);
        t->begin = n;

        return;
    }

    struct CharFreqNode *prev;
    for (; n; n = n->next)
    {
        if (n->c == c)
        {
            ++(n->val);
            return;
        }

        prev = n;
    }

    prev->next = construct_CharFreqNode(c, 1, NULL);
}

/**
 * @brief Prints the horizontal nodes of the tree in the 
 * form of a table.
 * 
 * @param t 
 */
void huffTree_print_table(HuffTree t)
{
    for (struct CharFreqNode *n = t.begin; n; n = n->next)
    {
        if (n->c == '\n')
        {
            printf("newline: %lu\n", n->val);
        }
        else
            printf("%c: %lu\n", n->c, n->val);
    }
}
/**
 * @brief Constructs a Hufftree of horizontal nodes, each representing
 * a character and the frequency it appears on the stream.
 * 
 * @param fp 
 * @return HuffTree 
 */
HuffTree huffTree_construct_f_stream(FILE *fp)
{
    HuffTree t;
    t.begin = NULL;

    for (char c = getc(fp); c != EOF; c = getc(fp))
    {
        huffTree_increase_freq(c, &t);
    }
    huffTree_increase_freq(C_ENDSTREAM, &t);

    return t;
}
/**
 * @brief Iteratively destroys the horizontal nodes of a tree.
 * 
 * WARNING: This function does not free the memory from the child nodes.
 * Only use it when the tree consists of a single level.
 * 
 * @param t 
 */
void huffTree_destroy(HuffTree t)
{

    for (struct CharFreqNode *n = t.begin->next; n; n = n->next)
    {
        free(t.begin);
        t.begin = n;
    }

    free(t.begin); //Free last node.
    t.begin = NULL;
}
/**
 * @brief Sort the horizontal nodes of the tree increasingly according
 * to their integer val (frequency).
 * 
 * Creates an array of all the CharFreqNode pointers of the horizontal
 * tree, then sorts this array using qsort() from <stdlib.h> and the CharFreqNode_compare()
 * function. Lastly, it rebuilds the tree following the
 * order of the array.
 * 
 * @param t 
 */
void huffTree_qsort(HuffTree *t)
{
    if (!t->begin)
        return;

    int size = huffTree_length(*t);

    struct CharFreqNode *huffTree_ptrs[size];

    {
        int i = 0;
        for (struct CharFreqNode *n = t->begin; n; n = n->next)
        {
            huffTree_ptrs[i] = n;
            ++i;
        }
    }

    qsort(huffTree_ptrs, size, sizeof(struct CharFreqNode *), CharFreqNode_compare);

    t->begin = huffTree_ptrs[0];
    huffTree_ptrs[size - 1]->next = NULL;
    struct CharFreqNode *n = t->begin;
    for (int i = 1; i < size; ++i)
    {
        n->next = huffTree_ptrs[i];
        n = n->next;
    }
}

void huffTree_insert_charFreqNode(struct CharFreqNode *n, char * code, struct CharFreqNode * parent)
{

    if(strcmp(code, "1") == 0) // Strings equal, right path
    {
        if(parent->nextr != NULL)
        {
            // TODO: Free node subtree
            free(parent->nextr);
        }

        parent->nextr = n;
        return;
    }

    if(strcmp(code, "0") == 0) // Strings equal, left path
    {
        if(parent->nextl != NULL)
        {
            // TODO: Free node subtree
            free(parent->nextl);
        }

        parent->nextl = n;
        return;
    }

    if(code[0] == '1')
    {
        if(!(parent->nextr))
        {
            struct CharFreqNode * new_node = construct_CharFreqNode(0, 0, NULL);
            parent->nextr = new_node;
            
        }
        huffTree_insert_charFreqNode(n, code + 1, parent->nextr);
        return;
    }
    if(code[0] == '0')
    {
        if(!(parent->nextl))
        {
            struct CharFreqNode * new_node = construct_CharFreqNode(0, 0, NULL);
            parent->nextl = new_node;
            
        }
        huffTree_insert_charFreqNode(n, code + 1, parent->nextl);
        return;

    }


}
