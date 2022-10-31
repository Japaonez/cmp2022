%{
#include <stdio.h>
#include <stdlib.h>

#include "header.h"

int yyerror(const char *s);
int yylex(void);

extern int yylineno;
%}

%union{
	token_args args;
	struct noh *no;
}

%define parse.error verbose

%token TOK_PRINT TOK_IF TOK_ELSE TOK_WHILE TOK_TRUE TOK_FALSE TAB
%token <args> TOK_IDENT TOK_INTEGER TOK_FLOAT
%start program

%type <no> program stmts stmt atribuicao aritmetica termo termo2 fator if logica ltermo lfator valor

%%

program : stmts { 
					noh *program = create_noh(PROGRAM, 1); program->children[0] = $1;

					print(program);

					//chamada da arvore abstrata
					//chamada da verificação semântica
					//chamada da geração de código
				}
		;

stmts : stmts stmt	{
						noh *n = $1;
						n = (noh*)realloc(n, sizeof(noh) + sizeof(noh*) * n->childcount);
						n->children[n->childcount] = $2;
						n->childcount++;
						$$ = n;
					}
	  | stmt	{ 
					$$ = create_noh(STMT, 1);
					$$->children[0] = $1;;
				}
	  ;

stmt : atribuicao	{
						$$ = $1;
					}
	 | TOK_PRINT aritmetica	{
								$$ = create_noh(PRINT, 1);
								$$->children[0] = $2;
							}
	 | TOK_IF '(' logica ')' ':' if {
													$$ = create_noh(IF, 2);
													$$->children[0] = $3;
													$$->children[1] = $6;
	 											}
	 | TOK_WHILE '(' logica ')' ':' TAB stmts {
													$$ = create_noh(WHILE, 2);
													$$->children[0] = $3;
													$$->children[1] = $7;
	 											}
	 ;

if : TAB stmts { $$ = $2; }
	 | TAB TOK_ELSE ':' stmts {
								$$ = create_noh(ELSE, 1);
								$$->children[0] = $4;
							}
	 ;

logica : logica '&''&' ltermo {
								$$ = create_noh(E, 2);
								$$->children[0] = $1;
								$$->children[1] = $4;
							}
	   | ltermo { $$ = $1; }
	   ;

ltermo : ltermo '|''|' lfator{
								$$ = create_noh(OU, 2);
								$$->children[0] = $1;
								$$->children[1] = $4;
							}
	   | lfator { $$ = $1; }
	   ;

lfator : '(' logica ')' { $$ = $2; }
		| valor '>' valor {
							$$ = create_noh(MAIOR, 2);
							$$->children[0] = $1;
							$$->children[1] = $3;
						}
		| valor '>''=' valor {
							$$ = create_noh(MAIORIGUAL, 2);
							$$->children[0] = $1;
							$$->children[1] = $4;
						}
		| valor '<' valor {
							$$ = create_noh(MENOR, 2);
							$$->children[0] = $1;
							$$->children[1] = $3;
						}
		| valor '<''=' valor {
							$$ = create_noh(MENORIGUAL, 2);
							$$->children[0] = $1;
							$$->children[1] = $4;
						}
		| valor '=''=' valor {
							$$ = create_noh(IGUAL, 2);
							$$->children[0] = $1;
							$$->children[1] = $4;
						}
		| valor '!''=' valor {
							$$ = create_noh(DIFERENTE, 2);
							$$->children[0] = $1;
							$$->children[1] = $4;
						}
		| valor { $$ = $1; }
	   ;

atribuicao : TOK_IDENT '=' aritmetica	{
											$$ = create_noh(ASSIGN, 2);
											$$->children[0] = create_noh(IDENT, 0);
											$$->children[0]->name = $1.ident;
											$$->children[1] = $3;
										}
		   ;

aritmetica : aritmetica '+' termo	{
										$$ = create_noh(SUM, 2);
										$$->children[0] = $1;
										$$->children[1] = $3;
									}
		   | aritmetica '-' termo	{
										$$ = create_noh(MINUS, 2);
										$$->children[0] = $1;
										$$->children[1] = $3;
										}
		   | termo	{
						$$ = $1;
					}
		   ;

termo : termo '*' termo2	{
								$$ = create_noh(MULTI, 2);
								$$->children[0] = $1;
								$$->children[1] = $3;
							}
	  | termo '/' termo2	{
								$$ = create_noh(DIVIDE, 2);
	  							$$->children[0] = $1;
	  							$$->children[1] = $3;
							}
	  | termo2	{
					$$ = $1;
				}
	  ;

termo2 : termo2 '^' fator	{
								$$ = create_noh(POW, 2);
								$$->children[0] = $1;
								$$->children[1] = $3;
							}
	   | fator	{ $$ = $1; }
	   ;

fator : '(' aritmetica ')'	{ $$ = $2; }
	  | valor { $$ = $1; }
	  ;

valor : TOK_IDENT	{
						$$ = create_noh(IDENT, 0);
	  					$$->name = $1.ident;
					}
	  | TOK_INTEGER	{
						$$ = create_noh(INTEGER, 0);
	  					$$->intv = $1.intv;
					}
	  | TOK_FLOAT	{
						$$ = create_noh(FLOAT, 0);
	  					$$->dblv = $1.dblv;
					}
	  | TOK_TRUE { $$ = create_noh(TRUE, 0); }
	  | TOK_FALSE { $$ = create_noh(FALSE, 0); }
	  ;

%%

int yyerror(const char *s){
	printf("Erro na linha %d: %s\n", yylineno, s);
	return 1;
}