/// @file strbin.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "codetable.h"
#include "charfreqnode.h"
#include "strbin.h"
#include "hufftree.h"

/**
 * @brief Byte, typedef unsigned char for bit manipulation.
 * 
 */
typedef unsigned char Byte;

/**
 * @brief Sets a bit of a byte depending on the passed char and the position.
 * 
 * e.g. set_bit(&b, '1', 3) will set the third bit in b from right to left to
 * 1. The starting position is 1, and the last position is 8.
 * 
 * @param b 
 * @param bit 
 * @param pos 
 */
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

/**
 * @brief Writes the encoded text to a stream.
 * 
 * Uses the passed CodeTable to encode the src test, and write it to dest.
 * 
 * @param src 
 * @param dest 
 * @param c_t 
 */
void write_encode_to_stream(FILE *src, FILE *dest, CodeTable c_t)
{
    write_codeTable_to_stream(dest, c_t);
    write_code_to_stream(src, dest, c_t);
}

/**
 * @brief Writes the second section of the compressed file.
 * 
 * The second section includes the literal translation of 
 * each character in the src file to its code, writing them to c_t.
 * During compression, a C_ENDSTREAM char was added to signal
 * the end of this section and end of the encoded file.
 * 
 * @param src 
 * @param dest 
 * @param c_t 
 */
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

/**
 * @brief Encodes the CodeTable to the dest file.
 * 
 * The first section of the encoded file, adds the metadata necessary
 * for reconstruction of the HuffTree during decompression.
 * 
 * The metadata consists of 3 parts per character. The character itself,
 * using 1 byte, the size of the character, using 5 bits, and the code of the character
 * in binary, using a variable amount of bytes up to 31 bits (limited by the size section's 5 bits).
 * 
 * @param dest 
 * @param c_t 
 */
void write_codeTable_to_stream(FILE *dest, CodeTable c_t)
{
    for (struct CodeNode *i_n = c_t.begin; i_n; i_n = i_n->next)
    {
        // First byte is the character
        fwrite(&(i_n->c), sizeof(char), 1, dest);

        // Next 5 bits is the code length (unsigned integer)
        Byte codesize = (Byte)strlen(i_n->code);
        Byte b = codesize;
        b = b << 3;

        // The rest bytes up to codesize-3 /8 is the code.
        int written_bytes = 1;

        int extra_bytes = (((int)codesize - 4) + 8) / 8;
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

/**
 * @brief Encodes the src file to the dest file.
 * 
 * Wrapper for write_encode_to_stream(). Handles paths to files instead of streams.
 * 
 * @param src_path 
 * @param dest_path 
 * @param c_t 
 */
void write_encode_to_file(char const *src_path, char const *dest_path, CodeTable c_t)
{

    FILE *src_p = fopen(src_path, "r");
    FILE *dest_p = fopen(dest_path, "wb");

    if (!src_p)
    {
        return;
    }
    if (!dest_p)
    {
        return;
    }

    write_encode_to_stream(src_p, dest_p, c_t);

    fclose(src_p);
    fclose(dest_p);
}

/**
 * @brief Constructs the code of the next character in the CodeTable metadata section of a compressed file.
 * 
 * Receives a recipient of the code as a string, reads the character of the first section of a compressed file,
 * and writes its code to the recipient 'holder' parameter, converting the binary code to string.
 * 
 * NOTE: Assumes the character byte has been read beforehand. Expects to read the byte containing the size
 * first.
 * 
 * @param holder 
 * @param src 
 */
void construct_code_string(char *holder, FILE *src)
{
    Byte size_byte; // byte that contains the size

    fread(&size_byte, sizeof(Byte), 1, src);
    int size = size_byte >> 3;

    int extra_bytes = ((size - 4) + 8) / 8;

    Byte base_b;
    int code_char = 0;

    for (int byte = 0; byte <= extra_bytes; ++byte)
    {

        if (byte == 0)
        {

            base_b = size_byte;

            for (int bit_pos = 2; bit_pos >= 0; --bit_pos) // grab only the last 3 bits of the first byte
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
}

/**
 * @brief Construct the HuffTree from a file stream.
 * 
 * Reads the first section (metadata) of the compressed file and reconstructs the HuffTree.
 * 
 * @param src 
 * @return HuffTree 
 */
HuffTree construct_huffTree_f_stream(FILE *src)
{
    HuffTree t;
    t.begin = NULL;

    while (1)
    {
        char c;

        fread(&c, sizeof(char), 1, src); // read the char in codeTable header.

        if (c == C_EOT)
        {
            break;
        }

        char code[CODE_MAX_LENGTH];

        construct_code_string(code, src);

        struct CharFreqNode *n = construct_CharFreqNode(c, 0, NULL);

        if (!t.begin)
        {
            t.begin = construct_CharFreqNode(0, 0, NULL);
        }
        huffTree_insert_charFreqNode(n, code, t.begin);
    }

    return t;
}

/**
 * @brief Reads the second part of the compressed file and  uses the HuffTree to decode it to dest.
 * 
 * Receives a HuffTree constructed with construct_huffTree_f_stream(), and decodes the second part
 * of the file to dest.
 * 
 * @param src 
 * @param dest 
 * @param t 
 */
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

/**
 * @brief Wrapper function for encoding (compression).
 * 
 * @param src_path 
 * @param dest_path 
 */
void encode_to_file(char *src_path, char *dest_path)
{
    FILE *src_fp = fopen(src_path, "r");

    if (!src_fp)
    {
        printf("Could not open the file for compression. Aborting!!! (check your privileges)\n");
        return;
    }

    puts("Analyzing character frequency...");
    HuffTree t = huffTree_construct_f_stream(src_fp);

    fclose(src_fp);

    puts("Sorting the characters by frequency...");
    huffTree_qsort(&t); // Sort the horizontal nodes by frequency to optimize tree construction

    puts("Constructing the binary tree...");
    huffTree_construct_tree(&t);

    puts("Constructing the code table...");
    CodeTable c_t = construct_CodeTable_f_tree(t); // Table of the codes for each char for encoding

    puts("Writing encoded text to target...");
    write_encode_to_file(src_path, dest_path, c_t);

    puts("Cleaning...");
    huffTree_nodeTree_deallocate(t.begin);
    codeTable_deallocate(c_t);

    puts("File compressed.");
}
/**
 * @brief Wrapper function for decoding (decompression).
 * 
 * @param src_path 
 * @param dest_path 
 */
void decode_to_file(char *src_path, char *dest_path)
{
    FILE *src_fp = fopen(src_path, "rb"); // compressed file in binary

    if (!src_fp)
    {
        printf("Error opening the compressed file. Aborting. (check that the file exists and you have the appropiate permissions)\n");
        return;
    }

    FILE *dest_fp = fopen(dest_path, "w"); // write in text

    if (!dest_fp)
    {
        printf("Error decompressing file to target. Aborting. (Check that you have the appropiate permissions to write to the folder)\n");
        return;
    }

    puts("Constructing the tree...");
    HuffTree t = construct_huffTree_f_stream(src_fp);

    puts("Decoding the text...");
    tree_decode_to_stream(src_fp, dest_fp, t);

    puts("Cleaning...");
    huffTree_nodeTree_deallocate(t.begin);

    puts("File decompressed.");
    fclose(src_fp);
    fclose(dest_fp);
}