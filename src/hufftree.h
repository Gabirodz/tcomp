#if !defined(HUFFTREE_H_)
#define HUFFTREE_H_

#include <stdio.h>

struct CharFreqNode;

typedef struct HuffTree
{
    struct CharFreqNode *begin;
} HuffTree;

void huffTree_print_table(HuffTree t);
int huffTree_length(HuffTree t);
unsigned long huffTree_get_freq(char c, HuffTree t);
void huffTree_enqueue(struct CharFreqNode *node, HuffTree *t);
void huffTree_instert_ordered_node(struct CharFreqNode *node, HuffTree *t);
struct CharFreqNode *huffTree_dequeue(HuffTree *t);
void huffTree_construct_tree(HuffTree *t);
int huffTree_length(HuffTree t);
void huffTree_increase_freq(char c, HuffTree *t);
void huffTree_print_table(HuffTree t);
HuffTree huffTree_construct_f_stream(FILE *fp);
void huffTree_destroy(HuffTree t);
void huffTree_qsort(HuffTree *t);

#endif // HUFFTREE_H_
