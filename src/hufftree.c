#include <stdio.h>
#include <stdlib.h>
#include "hufftree.h"
#include "charfreqnode.h"

unsigned long huffTree_get_freq(char c, HuffTree t)
{
    for (struct CharFreqNode *n = t.begin; n->next; n = n->next)
    {
        if (n->c == c)
            return n->val;
    }

    return 0;
}

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
        }
    }
}

struct CharFreqNode *huffTree_dequeue(HuffTree *t)
{
    if (!t->begin)
        return NULL;

    struct CharFreqNode *first_n = t->begin;
    t->begin = t->begin->next;

    return first_n;
}

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
        ++i;
    }
}

int huffTree_length(HuffTree t)
{
    if (t.begin == NULL)
        return 0;

    int count = 0;
    for (struct CharFreqNode *n = t.begin; n; n = n->next)
        ++count;

    return count;
}

void huffTree_increase_freq(char c, HuffTree *t)
{
    struct CharFreqNode *n = t->begin;

    if (!n)
    {
        puts("constr");
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

HuffTree huffTree_construct_f_stream(FILE *fp)
{
    HuffTree t;
    t.begin = NULL;

    for (char c = getc(fp); c != EOF; c = getc(fp))
    {
        huffTree_increase_freq(c, &t);
    }

    return t;
}

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