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

%token TOK_PRINT 
%token <args> TOK_IDENT TOK_INTEGER TOK_FLOAT
%start program

%type <no> program stmts stmt atribuicao aritmetica termo termo2 fator

%%

program : stmts { 
					noh *program = create_noh(PROGRAM, 1); program->children[0] = $1;

					print(program);
					debug();


					//chamada da arvore abstrata
					//chamada da verificação semântica
					visitor_leaf_first(&program, check_declared_vars);
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
	 ;

atribuicao : TOK_IDENT '=' aritmetica	{
											if (!simbolo_existe($1.ident))
												simbolo_novo($1.ident, TOK_IDENT);
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
	   | fator	{
					$$ = $1;
				}
	   ;

fator : '(' aritmetica ')'	{
								$$ = $2;
							}
	  | TOK_IDENT	{
						if (!simbolo_existe($1.ident))
							simbolo_novo($1.ident, TOK_IDENT);
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
	  ;

%%

int yyerror(const char *s){
	printf("Erro na linha %d: %s\n", yylineno, s);
	return 1;
}

simbolo *simbolo_novo(char *nome, int token){
	tsimbolos[simbolo_qnt].nome = nome;
	tsimbolos[simbolo_qnt].token = token;
	simbolo *result = &tsimbolos[simbolo_qnt];
	simbolo_qnt++;
	return result;
}

bool simbolo_existe(char *nome){
	// busca linear, nao eficiente
	for(int i = 0; i < simbolo_qnt; i++){
		if(strcmp(tsimbolos[i].nome, nome) == 0)
			return true;
	}
	return false;
}

void debug(){
	printf("Simbolos: \n");
	for(int i = 0; i < simbolo_qnt; i++)
		printf("\t%s\n", tsimbolos[i].nome);
}