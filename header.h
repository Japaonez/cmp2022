#pragma once

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
// header.h

enum noh_type {PROGRAM, STMT, GENERIC, ASSIGN, SUM, MINUS, MULTI, DIVIDE, PRINT, POW, FLOAT, PAREN, IDENT, INTEGER, IF, ELSE, WHILE, MAIOR, MAIORIGUAL, MENOR, MENORIGUAL, IGUAL, DIFERENTE, OU, E, TRUE, FALSE, ELSEIF, INT, CHAR, VAR, FLOAT2};

static const char *noh_type_names[] = {"programa", "stmt", "generico", "=", "+", "-", "*", "/", "print", "^", "float", "( )", "ident", "int", "if", "else", "while", ">", ">=", "<", "<=", "==", "!=", "||", "&&", "true", "false", "else if", "int", "char", "var", "float"};

typedef struct {
    int intv;
    double dblv;
    char *ident;
} token_args;

typedef struct {
	char *nome, *tipo;
	int token, linha, coluna;
	bool exists;
} simbolo;

static int simbolo_qnt = 0, error_count = 0, simbolo_qnt_erro = 0;
// simbolo tsimbolos[100];
// simbolo *simbolo_novo(char *nome, int token);
simbolo *simbolo_novo(char *nome, int token, int tipo);
bool simbolo_existe(char *nome);
void debug();

struct noh {
    int id, childcount, intv, linha, coluna;
    enum noh_type type;
    double dblv;
    char *name;

    struct noh *children[1];
};
typedef struct noh noh;

noh *create_noh(enum noh_type, int children);

void print(noh *root);
void print_rec(FILE *f, noh *root);

noh *tirar_stmt(noh *n, noh *m);

typedef void (*visitor_action)(noh **root, noh *no);

void check_declared_vars(noh **root, noh *no);
void visitor_leaf_first(noh **root, visitor_action act);
// void check_division_zero(noh *no);
void check_division_zero(noh **root, noh *no);
void check_type_incompatible(noh **root, noh *no);
void check_print(noh **root, noh *no);
void check_variable(noh **root, noh *no);

// simbolo *simbolo_erro(char *nome, int token);
void simbolo_erro(char *nome, int token, int n);
// simbolo *buscar_simbolo(noh *no);
simbolo *buscar_simbolo(char *nome);
void linha_coluna(simbolo *s);
int pegar_linha();
int pegar_coluna();
void error();