/// @file charfreqnode.c
#include <stdlib.h>
#include <stdio.h>
#include "charfreqnode.h"

/**
 * @brief Compares two CharFreqNode* for use with qsort.
 * 
 * @param a CharFreqNode* a
 * @param b CharFreqNode* b
 * @return int 
 */
int CharFreqNode_compare(const void *a, const void *b)
{
    return (*(struct CharFreqNode **)a)->val - (*(struct CharFreqNode **)b)->val;
}

/**
 * @brief Constructs a CharFreqNode* using dynamic memory allocation.
 * This function omits child node fields nextl and nextr
 * @param c character value
 * @param val integer value
 * @param next next horizontal node
 * @return struct CharFreqNode* 
 */
struct CharFreqNode *construct_CharFreqNode(char c, int val, struct CharFreqNode *next)
{
    struct CharFreqNode *n = malloc(sizeof(struct CharFreqNode));

    n->c = c;
    n->val = val;
    n->next = next;

    n->nextl = NULL;
    n->nextr = NULL;

    return n;
}

/**
 * @brief Prints the tree of a given node.
 * This function omits horizontal nodes and recursively
 * prints the children nodes of n.
 * 
 * @param n Parent node.
 */
void node_print_tree(struct CharFreqNode *n)
{
    if (!n)
        return;

    printf("parent: %d val: %d\n", (int)n->c, n->val);

    if (n->nextl)
    {
        printf("going left from %d \n", (int)n->c);
        node_print_tree(n->nextl);
    }
    if (n->nextr)
    {
        printf("going right from %d\n", (int)n->c);
        node_print_tree(n->nextr);
    }

    
}