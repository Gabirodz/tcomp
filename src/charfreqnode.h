#if !defined(CHARFREQNODE_H_)
#define CHARFREQNODE_H_

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

#endif // CHARFREQNODE_H_
