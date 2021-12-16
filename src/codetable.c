/// @file codetable.c
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "charfreqnode.h"
#include "hufftree.h"
#include "codetable.h"

/**
 * @brief Constructs a CodeNode
 *
 * Constructs a CodeNode using dynamic memory. The way to deallocate CodeNodes is
 * to deallocate their entire linked list with codeTable_deallocate().
 *
 * @param c
 * @param code
 * @param next
 * @return struct CodeNode*
 */
struct CodeNode *construct_CodeNode(char c, char *code, struct CodeNode *next)
{
    struct CodeNode *c_n = malloc(sizeof(struct CodeNode));
    c_n->c = c;
    c_n->code = malloc(sizeof(char) * CODE_MAX_LENGTH);

    // copy param code to node
    for (int i = 0;; ++i)
    {
        if (code[i] != 0) // copy until \0
        {
            c_n->code[i] = code[i];
        }
        else
        {
            c_n->code[i] = 0;
            break;
        }
    }
    c_n->next = next;

    return c_n;
}

/**
 * @brief Inserts a node in the CodeTable.
 *
 * Inserts a CodeNode at the end of the CodeTable.
 *
 * @param n
 * @param c_t
 */
void codeTable_insert(struct CodeNode *n, CodeTable *c_t)
{
    if (c_t->begin == NULL)
    {
        c_t->begin = n;
        return;
    }

    // insert at the end
    struct CodeNode *prev;
    for (struct CodeNode *c_ni = c_t->begin; c_ni; c_ni = c_ni->next)
    {
        prev = c_ni;
    }
    prev->next = n;
    // puts("out insert");
}

/**
 * @brief Get the character code of a given char.
 *
 * Get the character code of the given char, returns 0 if
 * not in the CodeTable.
 * @param c
 * @param c_t
 * @return char*
 */
char *find_char_code(char c, CodeTable c_t)
{
    if (c_t.begin)
    {
        for (struct CodeNode *i_n = c_t.begin; i_n; i_n = i_n->next)
        {
            if (i_n->c == c)
            {
                return i_n->code;
            }
        }
    }
    return 0;
}

/**
 * @brief Get the code of all the characters in a HuffTree, insert them to the CodeTable.
 *
 * Gets the code of all the characters from the parent n node.
 *
 * @param n The parent of the HuffTree.
 * @param t
 * @param code Should be passed an empty "" string.
 */
void code_build(struct CharFreqNode *n, CodeTable *t, char *code)
{
    if (!n->nextl && !n->nextr)
    {
        struct CodeNode *new_node = construct_CodeNode(n->c, code, NULL);
        codeTable_insert(new_node, t);
        return;
    }

    if (n->nextl)
    {
        // puts("before construct");

        char new_codel[strlen(code)];
        strcpy(new_codel, code);

        code_build(n->nextl, t, strcat(new_codel, "0"));
    }
    if (n->nextr)
    {
        char new_coder[strlen(code)];
        strcpy(new_coder, code);

        code_build(n->nextr, t, strcat(new_coder, "1"));
    }
}

/**
 * @brief Constructs the CodeTable of a given tree.
 *
 * Outer function for code_build(), returns the code_table with all the characters in it.
 * @param t
 * @return CodeTable
 */
CodeTable construct_CodeTable_f_tree(HuffTree t)
{
    CodeTable c_t;
    c_t.begin = NULL;

    if (t.begin)
    {
        code_build(t.begin, &c_t, "");
    }

    return c_t;
}

/**
 * @brief Deallocate all nodes of the CodeTable.
 *
 * @param c_t
 */
void codeTable_deallocate(CodeTable c_t)
{
    for (struct CodeNode *i_cn = c_t.begin; i_cn;)
    {
        struct CodeNode *next_ptr = i_cn->next;

        free(i_cn->code);
        free(i_cn);

        i_cn = next_ptr;
    }
}

/**
 * @brief Prints the CodeTable
 *
 * @param t
 */
void codeTable_print(CodeTable t)
{
    for (struct CodeNode *n = t.begin; n; n = n->next)
    {
        if (n->c == '\n')
            printf("newline: %s\n", n->code);
        else
            printf("%c: %s\n", n->c, n->code);
    }
}
