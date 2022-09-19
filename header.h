#pragma once

// header.h

enum noh_type {PROGRAM, ASSIGN, SUM, MINUS, MULTI, DIVIDE, PRINT, POW};

struct noh {
    int id, childcount;
    enum noh_type type;

    struct noh *filhos[1];
};
typedef struct noh noh;

noh *create_noh(enum noh_type, int filhos);