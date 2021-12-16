/// @file tcomputils.c
#include <stdio.h>

/**
 * @brief Prints the help text to console.
 * 
 */
void print_help(void)
{
    puts("tcomp <c/d> <source> \n"
         "<c/d>: c for compression, d for decompression\n"
         "<source>: file to compress, or compressed .t.mp file\n"
         "NOTE: It will overwrite\n"
         "any file with name <source><(decompressed).txt/.t.mp> depending on the command.");
}

/**
 * @brief Checks if a file is empty.
 * 
 * Receiving an empty file during compression or decompression may cause errors, so checking is necessary.
 * 
 * @param path 
 * @return int 
 */
int check_empty(char *path)
{
    FILE *fp = fopen(path, "r");

    if (!fp)
    {
        puts("Could not open file for compression. (Check that the file exists and you have the privileges to access it).");
        return 1;
    }
    else
    {
        char c = fgetc(fp);
        if (c == EOF)
        {
            puts("File is empty.");
            return 1;
        }
    }
    return 0;
}