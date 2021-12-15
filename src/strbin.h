#if !defined(STRBIN_H_)
#define STRBIN_H_

#include <stdio.h>

typedef struct CodeTable CodeTable;
typedef struct HuffTree HuffTree;

typedef unsigned char Byte;
void set_bit(Byte * b, char bit, int pos);
void write_encode_to_stream(FILE * src, FILE * dest, CodeTable c_t);
void write_code_to_stream(FILE * src, FILE *dest, CodeTable c_t);
void write_encode_to_file(char const* src, char const* dest_path, CodeTable c_t);
void write_codeTable_to_stream(FILE * dest, CodeTable c_t);

void write_decode_f_file(char const* src, char const* dest_path);
void write_decode_f_stream(FILE *src, FILE* dest);
HuffTree construct_huffTree_f_stream(FILE* src);
void tree_decode_to_stream(FILE * src, FILE * dest,  HuffTree t);
void encode_to_file(char *src_path, char *dest_path);
void decode_to_file(char * src_path, char * dest_path);



#endif // STRBIN_H_
