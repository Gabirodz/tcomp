/// @file hufftree.h
#if !defined(HUFFTREE_H_)
#define HUFFTREE_H_

#include <stdio.h>

struct CharFreqNode;

/**
 * @brief The main data structure of the program, multi-purpose data structure wrapper.
 * 
 * The HuffTree consists of a single field pointing at the beginning or
 * parent node of the tree. Each node in the tree has 3 possible connections to another node.
 * 
 * The "horizontal tree" refers to all the nodes connected horizontally to
 * the begin node of this structure. (i.e. the nodes references in the 'next' field of each node).
 * 
 * Each nodes has a left and right child.
 * 
 * This data structure serves initially as the Frequency table, when all
 * nodes are connected only horizontally and there are no child nodes.
 * 
 * After constructing the tree, the begin node of the tree is the parent node of
 * all the nodes in the tree, and all horizontal nodes are removed.
 * 
 */
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
