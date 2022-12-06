%{
#include <stdio.h>
#include <stdlib.h>

#include "header.h"

int yyerror(const char *s);
int yylex(void);

extern int yylineno;
int yycol;
%}

%union{
	token_args args;
	struct noh *no;
}

%define parse.error verbose

%token TOK_PRINT TOK_IF TOK_ELSE TOK_WHILE TOK_TRUE TOK_FALSE TOK_ELSEIF  TOK_DOUBLE TOK_CHAR
%token <args> TOK_IDENT TOK_INTEGER TOK_FLOAT TOK_INT TOK_float
%start program

%type <no> program stmts stmt atribuicao aritmetica termo termo2 fator if else logica ltermo lfator valor

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
					$$->children[0] = $1;
				}
	  ;

stmt : atribuicao	{
						$$ = $1;
					}
	 | TOK_PRINT aritmetica	{
								$$ = create_noh(PRINT, 1);
								$$->children[0] = $2;

								visitor_leaf_first(&$$, check_print);
							}
	 | TOK_IF if { $$ = $2; }
	 | TOK_WHILE '(' logica ')' '{' stmts '}' {
													noh *n = create_noh(WHILE, 1);
													$$ = tirar_stmt(n, $6);
													$$->children[0] = $3;
	 											}
	 ;

if : '(' logica ')' '{' stmts '}'  {
													noh *n = create_noh(IF, 1);
													$$ = tirar_stmt(n, $5);
													$$->children[0] = $2;
	 											}
	| '(' logica ')' '{' stmts '}' else {
													noh *n = create_noh(IF, 2);
													$$ = tirar_stmt(n, $5);
													$$->children[0] = $2;
													$$->children[$$->childcount - 1] = $7;
	 											}

else : TOK_ELSEIF '(' logica ')' '{' stmts '}' else {
								noh *n = create_noh(ELSEIF, 2);
								$$ = tirar_stmt(n, $6);
								$$->children[0] = $3;
								$$->children[$$->childcount - 1] = $8;
							}
	 | TOK_ELSE '{' stmts '}' {
								noh *n = create_noh(ELSE, 0);
								$$ = tirar_stmt(n, $3);
							}
	 ;

logica : logica '&''&' ltermo {
								$$ = create_noh(E, 2);
								$$->children[0] = $1;
								$$->children[1] = $4;
							}
	   | ltermo { $$ = $1; }
	   ;

ltermo : ltermo '|''|' lfator {
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

atribuicao : TOK_INT TOK_IDENT {
											if (!simbolo_existe($2.ident))
												simbolo_novo($2.ident, TOK_IDENT, INT);
											$$ = create_noh(VAR, 2);
											$$->children[0] = create_noh(INT, 0);
											// $$->children[0]->name = $1.ident;
											$$->children[1] = create_noh(IDENT, 0);
											$$->children[1]->name = $2.ident;
}
| TOK_float TOK_IDENT {
											if (!simbolo_existe($2.ident))
												simbolo_novo($2.ident, TOK_IDENT, FLOAT);
											$$ = create_noh(VAR, 2);
											$$->children[0] = create_noh(FLOAT2, 0);
											// $$->children[0]->name = $1.ident;
											$$->children[1] = create_noh(IDENT, 0);
											$$->children[1]->name = $2.ident;
}
| TOK_IDENT '=' aritmetica	{
											if (!simbolo_existe($1.ident))
												simbolo_erro($1.ident, TOK_IDENT, 1);
											$$ = create_noh(ASSIGN, 2);
											$$->children[0] = create_noh(IDENT, 0);
											$$->children[0]->name = $1.ident;
											$$->children[1] = $3;

											visitor_leaf_first(&$$, check_division_zero);
											visitor_leaf_first(&$$, check_type_incompatible);
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

								// check_division_zero($3);
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
						if (!simbolo_existe($1.ident))
							simbolo_erro($1.ident, TOK_IDENT, 0);
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

void linha_coluna(simbolo *s){
	/* printf("linha %d \n", yylineno); */
	s->linha = yylineno;
	s->coluna = yycol;
}

/* int pegar_linha(){
	return yylineno;
}

int pegar_coluna(){
	return yycol;
} */