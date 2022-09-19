%{
#include <stdio.h>

#include "header.h"

int yyerror(const char *s);
int yylex(void);

extern int yylineno;
%}

%define parse.error verbose

%token TOK_PRINTA TOK_IDENT TOK_INTEIRO TOK_REAL //TOK_LETRA
%start program

%%

program : stmts {}
        ;

stmts : stmt stmts
      | stmt
      ;

stmt : atribuicao
     | TOK_PRINTA aritmetica
     ;

atribuicao : TOK_IDENT '=' aritmetica
           ;

aritmetica : aritmetica '+' termo
           | aritmetica '-' termo
           | termo
           ;

termo : termo '*' termo2
      | termo '/' termo2
      | termo2
      ;

termo2 : termo2 '^' fator
       | fator
       ;

fator : '(' aritmetica ')'
      | TOK_IDENT 
      | TOK_INTEIRO
      | TOK_REAL
      ;

%%

int yyerror(const char *s){
    printf("Erro na linha %d: %s\n", yylineno, s);
    return 1;
}