%{
#include <stdio.h>

int yyerror(const char *s);
int yylex(void);
%}

%token TOK_PRINTA TOK_IDENT TOK_INTEIRO TOK_REAL TOK_LETRA
%start program

%%

program : stmts {}
        ;

stmts : stmt stmts
      | stmt
      ;

stmt : atribuicao
     | TOK_PRINTA 
     ;

atribuicao : TOK_IDENT '=' aritmetica
           ;

aritmetica : aritmetica '+' termo
           | termo
           ;

termo : termo '*' fator
      | fator
      ;

fator : '(' aritmetica ')'
      | TOK_IDENT
      | TOK_INTEIRO
      | TOK_REAL
      ;

%%

int yyerror(const char *s){
    printf("Erro: %s\n", s);
    return 1;
}