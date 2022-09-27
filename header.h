#pragma once

// header.h

enum noh_type {PROGRAM, STMT, GENERIC, ASSIGN, SUM, MINUS, MULTI, DIVIDE, PRINT, POW, FLOAT, PAREN, IDENT, INTEGER};

static const char *noh_type_names[] = {"programa", "stmt", "generico", "=", "+", "-", "*", "/", "print", "^", "float", "( )", "ident", "integer"};

typedef struct {
    int intv;
    double dblv;
    char *ident;
} token_args;

struct noh {
    int id, childcount, intv;
    enum noh_type type;
    double dblv;
    char *name;

    struct noh *children[1];
};
typedef struct noh noh;

noh *create_noh(enum noh_type, int children);

void print(noh *root);
void print_rec(FILE *f, noh *root);