#pragma once

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
// header.h

enum noh_type {PROGRAM, STMT, GENERIC, ASSIGN, SUM, MINUS, MULTI, DIVIDE, PRINT, POW, FLOAT, PAREN, IDENT, INTEGER};

static const char *noh_type_names[] = {"programa", "stmt", "generico", "=", "+", "-", "*", "/", "print", "^", "float", "( )", "ident", "integer"};

typedef struct {
    int intv;
    double dblv;
    char *ident;
} token_args;

typedef struct {
	char *nome;
	int token;
	bool exists;
} simbolo;

static int simbolo_qnt = 0;
simbolo tsimbolos[100];
simbolo *simbolo_novo(char *nome, int token);
bool simbolo_existe(char *nome);
void debug();

struct noh {
    int id, childcount, intv;
    enum noh_type type;
    double dblv;
    char *name;

    struct noh *children[1];
};
typedef struct noh noh;

typedef void (*visitor_action)(noh **root, noh *no);

void check_declared_vars(noh **root, noh *no);
void visitor_leaf_first(noh **root, visitor_action act);

noh *create_noh(enum noh_type, int children);

void print(noh *root);
void print_rec(FILE *f, noh *root);