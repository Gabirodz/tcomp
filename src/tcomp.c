#include <stdlib.h>
#include <stdio.h>
#include "hufftree.h"
#include "charfreqnode.h"
#include "codetable.h"

int main(int argc, char const *argv[])
{
    if (argc != 2)
        return 2;

    char const *path = argv[1];

    FILE *fp = fopen(path, "r");

    HuffTree t = huffTree_construct_f_stream(fp);

    huffTree_qsort(&t);
    huffTree_construct_tree(&t);
    puts("codetable------------");
    CodeTable c_t = construct_CodeTable_f_tree(t);
    codeTable_print(c_t);
    huffTree_destroy(t);

    return 0;
}
