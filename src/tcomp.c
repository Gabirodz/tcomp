#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "hufftree.h"
#include "charfreqnode.h"
#include "codetable.h"
#include "strbin.h"
#include "tcomputils.h"

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        print_help();
        return 1;
    }

    char *command = argv[1];
    char *const src_path = argv[2];

    if (strcmp(command, "c") == 0) // compress command
    {
        if (check_empty(src_path) != 0)
            return 1;
        char targetname[64];
        strcpy(targetname, src_path);
        char *dest_path = strcat(strtok(targetname, "."), ".t.mp");

        encode_to_file(src_path, dest_path);
    }
    else if (strcmp(command, "d") == 0) // decompress command
    {
        char targetname[64];
        strcpy(targetname, src_path);
        char *dest_path = strcat(strtok(targetname, "."), "(decompressed).txt");

        decode_to_file(src_path, dest_path);
    }
    else
    {
        print_help();
        return 1;
    }

    return 0;
}
