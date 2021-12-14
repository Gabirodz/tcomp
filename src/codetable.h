#if !defined(CODETABLE_H_)
#define CODETABLE_H_

#define CODE_MAX_LENGTH 12

struct CodeNode
{
    char c;
    char *code;
    struct CodeNode *next;
};

typedef struct CodeTable
{
    struct CodeNode *begin;
} CodeTable;

typedef struct HuffTree HuffTree;

struct CodeNode *construct_CodeNode(char c, char *code, struct CodeNode *next);
CodeTable construct_CodeTable_f_tree(HuffTree t);
void codeTable_insert(struct CodeNode *n, CodeTable *c_t);
void code_build(struct CharFreqNode *n, CodeTable *t, char *code);
void codeTable_print(CodeTable t);

#endif // CODETABLE_H_
