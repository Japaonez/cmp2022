%{
#include <stdio.h>

#include "header.h"

int yyerror(const char *s);
int yylex(void);

extern int yylineno;
%}

%union{
	struct noh *no;
}

%define parse.error verbose

%token TOK_PRINTA TOK_IDENT TOK_INTEIRO TOK_REAL
%start program

%type <no> program stmts stmt atribuicao aritmetica termo termo2 fator

%%

program : stmts { 
					noh *program = create_noh(PROGRAM, 1); program->children[0] = $1;

					print(program);

					//chamada da arvore abstrata
					//chamada da verificação semântica
					//chamada da geração de código
				}
		;

stmts : stmt stmts	{ $$ = create_noh(STMT, 2); $$->children[0] = $1; $$->children[1] = $2; }
	  | stmt	{ $$ = create_noh(STMT, 1); $$->children[0] = $1; }
	  ;

stmt : atribuicao	{ $$ = create_noh(GENERIC, 1); $$->children[0] = $1; }
	 | TOK_PRINTA aritmetica	{ $$ = create_noh(PRINT, 1); $$->children[0] = $2; }
	 ;

atribuicao : TOK_IDENT '=' aritmetica	{ $$ = create_noh(ASSIGN, 2); $$->children[0] = create_noh(IDENT, 0); $$->children[0]->name = NULL; $$->children[1] = $3; }
		   ;

aritmetica : aritmetica '+' termo	{ $$ = create_noh(SUM, 2); $$->children[0] = $1; $$->children[1] = $3; }
		   | aritmetica '-' termo	{ $$ = create_noh(MINUS, 2); $$->children[0] = $1; $$->children[1] = $3; }
		   | termo	{ $$ = create_noh(GENERIC, 1); $$->children[0] = $1; }
		   ;

termo : termo '*' termo2	{ $$ = create_noh(MULTI, 2); $$->children[0] = $1; $$->children[1] = $3; }
	  | termo '/' termo2	{ $$ = create_noh(DIVIDE, 2); $$->children[0] = $1; $$->children[1] = $3; }
	  | termo2	{ $$ = create_noh(GENERIC, 1); $$->children[0] = $1; }
	  ;

termo2 : termo2 '^' fator	{ $$ = create_noh(POW, 2); $$->children[0] = $1; $$->children[1] = $3; }
	   | fator	{ $$ = create_noh(GENERIC, 1); $$->children[0] = $1; }
	   ;

fator : '(' aritmetica ')'	{ $$ = create_noh(PAREN, 1); $$->children[0] = $2; }
	  | TOK_IDENT	{ $$ = create_noh(IDENT, 0); $$->name = NULL; }
	  | TOK_INTEIRO	{ $$ = create_noh(INTEGER, 0); $$->value = 0; }
	  | TOK_REAL	{ $$ = create_noh(FLOAT, 0); $$->value = 0; }
	  ;

%%

int yyerror(const char *s){
	printf("Erro na linha %d: %s\n", yylineno, s);
	return 1;
}