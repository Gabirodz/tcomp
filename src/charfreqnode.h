/// @file charfreqnode.h
#if !defined(CHARFREQNODE_H_)
#define CHARFREQNODE_H_

/**
 * @brief Node type for the HuffTree.
 * 
 * This type represents an ASCII character, an integer
 * value (to be used to store character frequency) and
 * a relation with 3 other nodes. The nextl and nextr
 * nodes represent the left and right childs of the node respectively,
 * and the next node represents an horizontal connection with another node,
 * which is needed for the initial sorting of the frequency list
 * and during the construction of the Huffman tree.
 * 
 */
struct CharFreqNode
{
    char c;
    unsigned long val;
    struct CharFreqNode *next;
    struct CharFreqNode *nextl;
    struct CharFreqNode *nextr;
};

struct CharFreqNode *construct_CharFreqNode(char c, int val, struct CharFreqNode *next);
void node_print_tree(struct CharFreqNode *n);
int CharFreqNode_compare(const void *a, const void *b);
void huffTree_nodeTree_deallocate(struct CharFreqNode *n);

#endif // CHARFREQNODE_H_
