//header.c
#include <stdlib.h>

#include "header.h"

noh *create_noh(enum noh_type nt, int filhos){
    static int IDCOUNT = 0;
    noh *newn = (noh*)calloc(1, sizeof(noh) + sizeof(noh*) * (filhos - 1));

    newn->type = nt;
    newn->childcount = filhos;
    newn->id = IDCOUNT++;
    return newn;
}