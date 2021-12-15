#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "hufftree.h"
#include "charfreqnode.h"
#include "codetable.h"
#include "strbin.h"

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        puts("tcomp <c/d> <source> ");
        puts("<c/d>: c for compression, d for decompression");
        puts("<source>: file to compress, or compressed .t.mp file");
        puts("NOTE: It will overwrite\n"
             "any file with name <source><(decompressed).txt/.t.mp> depending on the command.");
        return 2;
    }

    char *command = argv[1];
    char *const src_path = argv[2];

    if (strcmp(command, "c") == 0)
    {

        char targetname[64];
        strcpy(targetname, src_path);
        char *dest_path = strcat(strtok(targetname, "."), ".t.mp");

        encode_to_file(src_path, dest_path);
        
        
    }
    else if (strcmp(command, "d") == 0)
    {
        char targetname[64];
        strcpy(targetname, src_path);
        char *dest_path = strcat(strtok(targetname, "."), "(decompressed).txt");

        decode_to_file(src_path, dest_path);
    } else
    {
        puts("tcomp <c/d> <source> ");
        puts("<c/d>: c for compression, d for decompression");
        puts("<source>: file to compress, or compressed .t.mp file");
        puts("NOTE: It will overwrite\n"
             "any file with name <source><(decompressed).txt/.t.mp> depending on the command.");
        return 2;

    }

    return 0;
}
