/// @file codetable.h
#if !defined(CODETABLE_H_)
#define CODETABLE_H_

#define CODE_MAX_LENGTH 31

/**
 * @brief Node for the CodeTable
 * 
 * Basic node of a linked list, includes a reference to the next node,
 * and two pieces of data. The ASCII character and its code.
 * 
 */
struct CodeNode
{
    char c;
    char *code;
    struct CodeNode *next;
};

/**
 * @brief Linked list.
 * 
 * The CodeTable is a linked list of CodeNodes, serves simply as the 
 * entrypoint and contains only a reference to the first CodeNode.
 * 
 */
typedef struct CodeTable
{
    struct CodeNode *begin;
} CodeTable;

typedef struct HuffTree HuffTree;

struct CodeNode *construct_CodeNode(char c, char *code, struct CodeNode *next);
CodeTable construct_CodeTable_f_tree(HuffTree t);
void codeTable_insert(struct CodeNode *n, CodeTable *c_t);
void code_build(struct CharFreqNode *n, CodeTable *t, char *code);
char *find_char_code(char c, CodeTable c_t);
void codeTable_print(CodeTable t);
void codeTable_deallocate(CodeTable c_t);

#endif // CODETABLE_H_
