#pragma once

// header.h

enum noh_type {PROGRAM, STMT, GENERIC, ASSIGN, SUM, MINUS, MULTI, DIVIDE, PRINT, POW, FLOAT, PAREN, IDENT, INTEGER};

struct noh {
    int id, childcount;
    enum noh_type type;
    double value;
    char *name;

    struct noh *children[1];
};
typedef struct noh noh;

noh *create_noh(enum noh_type, int children);