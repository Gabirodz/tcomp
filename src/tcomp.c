#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "hufftree.h"
#include "charfreqnode.h"
#include "codetable.h"
#include "strbin.h"

int main(int argc, char *argv[])
{
    if (argc < 3 || 4 < argc)
    {
        puts("tcomp <c/d> <source> <target>");
        puts("<c/d>: c for compression, d for decompression");
        puts("<source>: file to compress, or compressed .t.mp file");
        puts("<target>: output, can be left blank");
        puts("NOTE: IF <target> left blank, it will overwrite\n"
             "any file with name <source>.<txt/t.mp> depending on the command.");
        return 2;
    }

    char *command = argv[1];
    char * const src_path = argv[2];

    if (strcmp(command, "c") == 0)
    {
        FILE *src_fp = fopen(src_path, "r");

        HuffTree t = huffTree_construct_f_stream(src_fp);

        fclose(src_fp);
        huffTree_qsort(&t);
        
        huffTree_construct_tree(&t);
        CodeTable c_t = construct_CodeTable_f_tree(t);
        codeTable_print(c_t);
        puts("end maker tree");
        char targetname[64];
        strcpy(targetname, src_path);
        char const *dest_path = strcat(strtok(targetname, "."), ".t.mp");
        write_encode_to_file(src_path, dest_path, c_t);
        huffTree_destroy(t);
    }
    else if (strcmp(command, "d") == 0)
    {
        FILE *src_fp = fopen(src_path, "rb");
        char *dest_path = strcat(strtok(src_path, "."), ".txt");
        puts("constructinf tree");
        HuffTree t = construct_huffTree_f_stream(src_fp);
        CodeTable c_t =  construct_CodeTable_f_tree(t);
        codeTable_print(c_t);
        

        
    }

    return 0;
}
