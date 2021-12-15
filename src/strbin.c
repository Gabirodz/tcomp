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
            
            fwrite(&b, sizeof(char), 1, dest);
            return;
        }
    }
}

void write_codeTable_to_stream(FILE *dest, CodeTable c_t)
{
    for (struct CodeNode *i_n = c_t.begin; i_n; i_n = i_n->next)
    {
        puts("begined loop;");
        // First byte is the character
        fwrite(&(i_n->c), sizeof(char), 1, dest);

        // Next 5 bits is the code length (unsigned integer)
        Byte codesize = (Byte)strlen(i_n->code);
        Byte b = codesize;
        b = b << 3;
        puts("assigned initial size");

        // The rest bytes up to codesize-3 /8 is the code.
        int written_bytes = 1;

        int extra_bytes = (((int)codesize - 4) + 8) / 8;
        printf("char: %c, extrabytes: %d, size = %d\n", i_n->c, extra_bytes, (int)codesize);
        int char_pos = 0;

        for (int written_bytes = 0; written_bytes <= extra_bytes; ++written_bytes)
        {
            int endchar = 0;
            if (written_bytes == 0) // first 5 bits are already used by the size
            {
                for (int bit_pos = 3; bit_pos >= 1; --bit_pos)
                {
                    if (i_n->code[char_pos] == '\0')
                    {
                        endchar = 1;
                        break;
                    }
                    set_bit(&b, i_n->code[char_pos], bit_pos);
                    ++char_pos;
                }
                fwrite(&b, sizeof(Byte), 1, dest);
            }
            else // The next bytes are free
            {
                b = 0;
                for (int bit_pos = 8; bit_pos >= 1; --bit_pos)
                {
                    if (i_n->code[char_pos] == '\0')
                    {
                        endchar = 1;
                        break;
                    }
                    set_bit(&b, i_n->code[char_pos], bit_pos);
                    ++char_pos;
                }
                fwrite(&b, sizeof(Byte), 1, dest);
            }

            if (endchar == 1)
            {
                break;
            }
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

void construct_code_string(char *holder, FILE *src) // -------------PROBELM--------=
{
    Byte size_byte; // byte that contains the size

    fread(&size_byte, sizeof(Byte), 1, src);
    int size = size_byte >> 3;

    if (size > 12)
    {
        puts("-------------------------------warning large size-----------------");
    }
    printf("received size; %d\n", size);

    int extra_bytes = ((size - 4) + 8) / 8;

    printf("extrabais: %d\n", extra_bytes);
    Byte base_b;
    int code_char = 0;

    for (int byte = 0; byte <= extra_bytes; ++byte)
    {
        printf("run: %d\n", byte);

        if (byte == 0)
        {

            base_b = size_byte;

            for (int bit_pos = 2; bit_pos >= 0; --bit_pos) // grab only the last 3 bits of the first byte
            {
                if (code_char == size)
                    break;

                // printf("bit_pos: %d\n", bit_pos);
                int bit = (base_b & (1 << bit_pos)) >> bit_pos;
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
        }
        else if (byte > 0)
        {

            fread(&base_b, sizeof(Byte), 1, src);

            for (int bit_pos = 7; bit_pos >= 0; --bit_pos) 
            {
                if (code_char == size)
                    break;

                int bit = (base_b & (1 << bit_pos)) >> bit_pos;
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
        }
    }

    holder[code_char] = '\0';
    puts(holder);
}

HuffTree construct_huffTree_f_stream(FILE *src)
{
    HuffTree t;
    t.begin = NULL;

    while (1)
    {
        puts("Executing construct_hufftree");
        char c;

        fread(&c, sizeof(char), 1, src); // read the char in codeTable header.

        puts("ended readchar");
        printf("read char: %c\n", c);
        if (c == C_EOT)
        {
            puts("fouind endstream");
            break;
        }

        puts("ended readchar");
        char code[CODE_MAX_LENGTH];

        puts("begin construct");
        construct_code_string(code, src); // -------------PROBELM--------=// -------------PROBELM--------=
        puts("end construct");

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
                    printf("found char: %c  (%d)\n", follow_ptr->c, (int)follow_ptr->c);

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
                    printf("found char: %c  (%d)\n", follow_ptr->c, (int)follow_ptr->c);

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

    if (!src_fp)
    {
        printf("Could not open the file for compression. Aborting!!! (check your privileges)\n");
        return;
    }

    HuffTree t = huffTree_construct_f_stream(src_fp);

    fclose(src_fp);

    huffTree_qsort(&t); // Sort the horizontal nodes by frequency to optimize tree construction
    huffTree_construct_tree(&t);
    CodeTable c_t = construct_CodeTable_f_tree(t); // Table of the codes for each char for encoding
    puts("before encode");
    write_encode_to_file(src_path, dest_path, c_t);
    puts("----codetable----");
    codeTable_print(c_t);

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
    puts("constructed huffman tree");

    tree_decode_to_stream(src_fp, dest_fp, t);
}