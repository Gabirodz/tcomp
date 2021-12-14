#include <stdlib.h>
#include <stdio.h>
#include "hufftree.h"
#include "charfreqnode.h"
#include "codetable.h"
#include "strbin.h"

int main(int argc, char const *argv[])
{
    if (argc != 3)
        return 2;

    char const *src_path = argv[1];
    char const *dest_path = argv[2];

    FILE *src_fp = fopen(src_path, "r");

    HuffTree t = huffTree_construct_f_stream(src_fp);

    fclose(src_fp);
    huffTree_qsort(&t);
    puts("freqtable--------");
    huffTree_print_table(t);
    huffTree_construct_tree(&t);
    puts("codetable------------");
    CodeTable c_t = construct_CodeTable_f_tree(t);
    codeTable_print(c_t);
    write_encode_to_file(src_path, dest_path, c_t);
    huffTree_destroy(t);

    return 0;
}
