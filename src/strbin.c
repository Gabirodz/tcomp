#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "codetable.h"
#include "charfreqnode.h"
#include "strbin.h"
#include "hufftree.h"

typedef unsigned char Byte;

void set_bit(Byte *b, char bit, int pos)
{
    if (pos > 8)
        pos = 8;

    if (bit == '1')
    {
        unsigned char x = 1;
        x = x << pos - 1;

        *b = *b | x;
    }
}

void write_encode_to_stream(FILE *src, FILE *dest, CodeTable c_t)
{
    write_codeTable_to_stream(dest, c_t);
    write_code_to_stream(src, dest, c_t);
}

void write_code_to_stream(FILE *src, FILE *dest, CodeTable c_t)
{
    unsigned long bits_used = 0;
    Byte b = 0;

    for (char c = getc(src);; c = getc(src))
    {

        int trigger_exit = 0;

        if (c == EOF)
        {
            c = C_ENDSTREAM;
            trigger_exit = 1;
        }

        char *code = find_char_code(c, c_t);
        for (int i = 0;; ++i)
        {
            if (bits_used == 8)
            {
                bits_used = 0;
                fwrite(&b, sizeof(Byte), 1, dest);
                b = 0;
            }

            if (code[i] != '\0')
            {

                set_bit(&b, code[i], 8 - bits_used);
                ++bits_used;
            }
            else
                break;
        }

        if (trigger_exit == 1)
        {
            if (bits_used != 7) // write last byte incomplete
            {
                fwrite(&b, sizeof(char), 1, dest);
            }
            return;
        }
    }
}

void write_codeTable_to_stream(FILE *dest, CodeTable c_t)
{
    for (struct CodeNode *i_n = c_t.begin; i_n; i_n = i_n->next)
    {
        // First byte is the character
        fwrite(&(i_n->c), sizeof(char), 1, dest);

        // Next 4 bits is the code length (unsigned integer)
        Byte codesize = (Byte)strlen(i_n->code);
        Byte b = codesize;
        b = b << 4;

        // The rest, up to 12 bits, is the code.
        int written_bytes = 1;

        for (int i = 12, j = 0;; --i, ++j)
        {
            if (i_n->code[j] == '\0')
            {
                fwrite(&b, sizeof(Byte), 1, dest);
                ++written_bytes;
                break;
            }
            if ((i % 8) == 0)
            {
                fwrite(&b, sizeof(Byte), 1, dest);
                ++written_bytes;

                b = 0;
                set_bit(&b, i_n->code[j], 8);
            }
            set_bit(&b, i_n->code[j], (i % 8));
        }

        if (written_bytes != 3)
        {
            Byte zero = 0;
            fwrite(&zero, sizeof(Byte), 1, dest);
        }
    }

    Byte end = (Byte)C_EOT;
    fwrite(&end, sizeof(Byte), 1, dest);
    return;
}

void write_encode_to_file(char const *src_path, char const *dest_path, CodeTable c_t)
{

    FILE *src_p = fopen(src_path, "r");
    FILE *dest_p = fopen(dest_path, "wb");

    if (!src_p)
    {
        puts("Could not open file for compr");
        return;
    }
    if (!dest_p)
    {
        printf("Error writing compressed file. Aborting...\n");
        exit(1); // ------- checar- --- -
    }

    write_encode_to_stream(src_p, dest_p, c_t);

    fclose(src_p);
    fclose(dest_p);
}

void construct_code_string(Byte b1, Byte b2, char *holder)
{
    int size = b1 >> 4;
    Byte base_b = b1;

    int code_char = 0;
    int initial_i = 12;
    for (int i = initial_i; i > initial_i - size; --i)
    {
        int pos = i % 8;
        if (pos == 0)
        {
            pos = 8;
            base_b = b2;
        }

        int bit = (base_b & (1 << pos - 1)) >> pos - 1; // pos in logic 1

        if (bit == 1)
        {
            holder[code_char] = '1';
        }
        else
        {
            holder[code_char] = '0';
        }
        ++code_char;
    }

    holder[code_char] = '\0';
}

HuffTree construct_huffTree_f_stream(FILE *src)
{
    HuffTree t;
    t.begin = NULL;

    while (1)
    {
        char c;

        fread(&c, sizeof(char), 1, src);

        if (c == C_EOT)
            break;

        Byte b1;
        Byte b2;
        fread(&b1, sizeof(Byte), 1, src);
        fread(&b2, sizeof(Byte), 1, src);

        char code[CODE_MAX_LENGTH];

        construct_code_string(b1, b2, code);

        struct CharFreqNode *n = construct_CharFreqNode(c, 0, NULL);

        if (!t.begin)
        {
            t.begin = construct_CharFreqNode(0, 0, NULL);
        }
        huffTree_insert_charFreqNode(n, code, t.begin);
    }

    return t;
}

void tree_decode_to_stream(FILE *src, FILE *dest, HuffTree t)
{

    struct CharFreqNode *topnode = t.begin;
    struct CharFreqNode *follow_ptr = topnode;
    int found_endstream = 0;

    Byte b;
    while (!found_endstream)
    {
        fread(&b, sizeof(Byte), 1, src);

        for (int i = 7; i >= 0; --i)
        {
            int bit = (b & (1 << i)) >> i; // ith bit in b

            if (bit == 0) // LEFT PATH
            {
                if (!follow_ptr->nextl)
                {
                    if (follow_ptr->c == C_ENDSTREAM)
                    {
                        found_endstream = 1;
                        break;
                    }
                    fprintf(dest, "%c", follow_ptr->c);
                    follow_ptr = topnode;
                }
                follow_ptr = follow_ptr->nextl;
            }

            if (bit == 1) // RIGHT PATH
            {
                if (!follow_ptr->nextr)
                {
                    if (follow_ptr->c == C_ENDSTREAM)
                    {
                        found_endstream = 1;
                        break;
                    }
                    fprintf(dest, "%c", follow_ptr->c);
                    follow_ptr = topnode;
                }
                follow_ptr = follow_ptr->nextr;
            }
        }
    }
}
void encode_to_file(char *src_path, char *dest_path)
{
    FILE *src_fp = fopen(src_path, "r");

    if(!src_fp)
    {
        printf("Could not open the file for compression. Aborting!!! (check your privileges)\n");
        return;
    }

    HuffTree t = huffTree_construct_f_stream(src_fp);

    fclose(src_fp);

    huffTree_qsort(&t); //Sort the horizontal nodes by frequency to optimize tree construction
    huffTree_construct_tree(&t);
    CodeTable c_t = construct_CodeTable_f_tree(t); // Table of the codes for each char for encoding
    write_encode_to_file(src_path, dest_path, c_t);

    huffTree_nodeTree_deallocate(t.begin);
    codeTable_deallocate(c_t);
    
}

void decode_to_file(char *src_path, char *dest_path)
{
    FILE *src_fp = fopen(src_path, "rb");  // compressed file in binary
    FILE *dest_fp = fopen(dest_path, "w"); // write in text

    if (!src_fp)
    {
        printf("Error opening the compressed file. Aborting!!! (check that the file exists and you have the appropiate permissions)\n");
        return;
    }
    if (!dest_fp)
    {
        printf("Error decompressing file. Aborting!!! (check if you have the appropiate permissions to write to <target>)\n");
        return;
    }

    HuffTree t = construct_huffTree_f_stream(src_fp);

    tree_decode_to_stream(src_fp, dest_fp, t);
}