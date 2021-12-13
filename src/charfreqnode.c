#include <stdlib.h>
#include <stdio.h>
#include "charfreqnode.h"

int CharFreqNode_compare(const void *a, const void *b)
{
    return (*(struct CharFreqNode **)a)->val - (*(struct CharFreqNode **)b)->val;
}

struct CharFreqNode *construct_CharFreqNode(char c, int val, struct CharFreqNode *next)
{
    struct CharFreqNode *n = malloc(sizeof(struct CharFreqNode));

    n->c = c;
    n->val = val;
    n->next = next;

    return n;
}

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