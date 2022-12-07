//header.c
#include <stdlib.h>

#include "header.h"

extern int yylineno, yycol;
simbolo tsimbolos[100];

noh *create_noh(enum noh_type nt, int filhos){
	static int IDCOUNT = 0;
	noh *newn = (noh*)calloc(1, sizeof(noh) + sizeof(noh*) * (filhos - 1));

	newn->type = nt;
	newn->childcount = filhos;
	newn->id = IDCOUNT++;
	newn->linha = yylineno;
	newn->coluna = yycol;

	return newn;
}

void print(noh *root){
	FILE *f = fopen("output.dot", "w");

	fprintf(f, "graph G{\n");
	print_rec(f, root);
	fprintf(f, "}");
	
	fclose(f);
}

noh *tirar_stmt(noh *n, noh *m){
	n = (noh*)realloc(n, sizeof(noh) + sizeof(noh*) * (m->childcount + n->childcount));

	if(n->childcount == 0){
		for(int i = 0; i < m->childcount; i++){
			n->children[i] = m->children[i];
			n->childcount++;
		}
	}else if(m->type == STMT){
		for(int i = 0; i < m->childcount; i++){
			n->children[i + 1] = m->children[i];
			n->childcount++;
		}
	}else
		n->children[1] = m;

	return n;
}

const char *get_label(noh *no){
	static char aux[100];
	switch(no->type){
		case INTEGER:
			sprintf(aux, "%d", no->intv);
			return aux;
		case FLOAT:
			sprintf(aux, "%f", no->dblv);
			return aux;
		case IDENT:
			return no->name;
		default:
			return noh_type_names[no->type];
	}
}

void print_rec(FILE *f, noh *root){
	fprintf(f, "\tN%d[label=\"%s\"];\n", root->id, get_label(root));
	for(int i = 0; i < root->childcount; i++){
		print_rec(f, root->children[i]);
		fprintf(f, "\tN%d -- N%d;\n", root->id, root->children[i]->id);
	}
}

int search_symbol(char *nome){
	// busca linear, nao eficiente
	for(int i = 0; i < simbolo_qnt; i++){
		if(strcmp(tsimbolos[i].nome, nome) == 0)
			return i;
	}
	return -1;
}

void check_declared_vars(noh **root, noh *no){
	noh *nr = *root;
	if(no->type == ASSIGN){
		int s = search_symbol(no->children[0]->name);
		if(s != -1)
			tsimbolos[s].exists = true;
	}

}

void check_division_zero(noh **root, noh *no){
	if(no->type == DIVIDE && no->children[1]->type == INTEGER && no->children[1] && no->children[1]->intv == 0){
		printf("%d.%d: \t", no->linha - 1, no->coluna);
		error();
		printf("divisão por zero.\n");
		error_count++;
	}
}

void check_type_incompatible(noh **root, noh *no){
	switch (no->type){
	case SUM:
	case MINUS:
	case DIVIDE:
	case MULTI:
	case POW:
		if((no->children[0]->type == INTEGER && no->children[1]->type == FLOAT) || (no->children[1]->type == INTEGER && no->children[0]->type == FLOAT)){
			printf("%d.%d: \t", yylineno - 1, yycol);
			error();
			printf("tipos incompativeis (%s %s %s).\n", noh_type_names[no->children[0]->type], noh_type_names[no->type], noh_type_names[no->children[1]->type]);
			error_count++;
		}
		if(no->children[1]){
			if(no->children[0]->name && no->children[1]->name && simbolo_existe(no->children[0]->name) && simbolo_existe(no->children[1]->name)){
				simbolo *s1 = buscar_simbolo(no->children[0]->name), *s2 = buscar_simbolo(no->children[1]->name);
				if(s1 != s2){
					printf("%d.%d: \t1", yylineno - 1, yycol);
					error();
					printf("tipos incompativeis ({%s}%s %s {%s}%s).\n", s2->nome, s2->tipo, noh_type_names[no->type], s1->nome, s1->tipo);
				error_count++;
				}
			}else if(no->children[0]->name && simbolo_existe(no->children[0]->name) && 
			(strcmp(buscar_simbolo(no->children[0]->name)->tipo, "int") == 0|| strcmp(buscar_simbolo(no->children[0]->name)->tipo, "float") == 0) && 
			(strcmp(noh_type_names[no->children[1]->type], "int") == 0|| strcmp(noh_type_names[no->children[1]->type], "float") == 0)){
				simbolo *s1 = buscar_simbolo(no->children[0]->name);
				if(strcmp(s1->tipo, noh_type_names[no->children[1]->type]) != 0){
					printf("%d.%d: \t2", yylineno - 1, yycol);
					error();
					printf("tipos incompativeis ({%s}%s %s %s).\n", s1->nome, s1->tipo, noh_type_names[no->type], noh_type_names[no->children[1]->type]);
				error_count++;
				}
			}else if(no->children[1]->name && simbolo_existe(no->children[1]->name) && 
			(strcmp(buscar_simbolo(no->children[1]->name)->tipo, "int") == 0|| strcmp(buscar_simbolo(no->children[1]->name)->tipo, "float") == 0) && 
			(strcmp(noh_type_names[no->children[0]->type], "int") == 0|| strcmp(noh_type_names[no->children[0]->type], "float") == 0)){
				simbolo *s2 = buscar_simbolo(no->children[1]->name);
				if(strcmp(s2->tipo, noh_type_names[no->children[0]->type]) != 0){
					printf("%d.%d: \t3", yylineno - 1, yycol);
					error();
					printf("tipos incompativeis (%s %s {%s}%s).\n", noh_type_names[no->children[0]->type], noh_type_names[no->type], s2->nome, s2->tipo);
				error_count++;
				}
			}
		}
		break;
	}
}

void check_print(noh **root, noh *no){
	if(no->type == TRUE || no->type == FALSE){
		printf("%d.%d: \t", no->linha, no->coluna);
		error();
		printf("não pode printar booleano.\n");
		error_count++;
	}
}

void check_variable(noh **root, noh *no){
	noh *teste = *root;
	static simbolo *var, *aux, *limpar;
	static char nome[10];

	if(no->type == ASSIGN){
		var = buscar_simbolo(no->children[0]->name);
		if(no->children[1]->name){
			aux = buscar_simbolo(no->children[1]->name);
			if(strcmp(var->tipo, aux->tipo) != 0){
				printf("%d.%d: \t", no->linha - 1, no->coluna);
				error();
				printf("{%s}%s não pode receber {%s}%s\n", var->nome, var->tipo, aux->nome, aux->tipo);
				error_count++;
			}
		}else if(no->children[1]->type == INTEGER || no->children[1]->type == FLOAT){
			if(strcmp(var->tipo, noh_type_names[no->children[1]->type]) != 0){
				printf("%d.%d: \t", no->linha, no->coluna);
				error();
				printf("{%s}%s não pode receber %s\n", var->nome, var->tipo, noh_type_names[no->children[1]->type]);
				error_count++;
			}
		}
	}
}

simbolo *buscar_simbolo(char *nome){
	for(int i = 0; i < simbolo_qnt; i++){
		if(strcmp(nome, tsimbolos[i].nome) == 0){
			return &tsimbolos[i];
		}
	}
}

void visitor_leaf_first(noh **root, visitor_action act){
	noh *r = *root;
	for(int i = 0; i < r->childcount; i++){
		visitor_leaf_first(&r->children[i], act);
		if(act)
			act(root, r->children[i]);
	}
}



simbolo *simbolo_novo(char *nome, int token, int tipo){
	tsimbolos[simbolo_qnt].nome = nome;
	tsimbolos[simbolo_qnt].token = token;
	tsimbolos[simbolo_qnt].exists = false;
	if(tipo == INT)
		tsimbolos[simbolo_qnt].tipo = "int";
	else
		tsimbolos[simbolo_qnt].tipo = "float";
	simbolo *result = &tsimbolos[simbolo_qnt];
	simbolo_qnt++;
	linha_coluna(result);
	return result;
}

void simbolo_erro(char *nome, int token, int n){
	printf("%d.%d: \t", yylineno - n, yycol);
	error();
	printf("símbolo %s não declarado.\n", nome);
	error_count++;
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
	printf("Simbolos: ");
	for(int i = 0; i < simbolo_qnt; i++)
		printf("%d.{%s}%s ", tsimbolos[i].linha, tsimbolos[i].nome, tsimbolos[i].tipo);
	printf("\nErros: %d\n", error_count);
}

void error(){
	printf("\033[0;31m");
	printf("error: ");
	printf("\033[0m");
}