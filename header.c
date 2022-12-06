//header.c
#include <stdlib.h>

#include "header.h"

extern int yylineno, yycol;

noh *create_noh(enum noh_type nt, int filhos){
	static int IDCOUNT = 0;
	noh *newn = (noh*)calloc(1, sizeof(noh) + sizeof(noh*) * (filhos - 1));

	newn->type = nt;
	newn->childcount = filhos;
	newn->id = IDCOUNT++;
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
		// printf("%d\n", simbolo_qnt);
		if(strcmp(tsimbolos[i].nome, nome) == 0)
			return i;
	}
	return -1;
}

void check_declared_vars(noh **root, noh *no){
	noh *nr = *root;
	if(no->type == ASSIGN){
		// printf("NOME %s\n", no->children[0]->name);
		int s = search_symbol(no->children[0]->name);
		if(s != -1)
			tsimbolos[s].exists = true;
	// }else if(no->type == IDENT && nr->type != ASSIGN){
		// if(nr->type != ASSIGN && no == nr->children[0])
		// 	return;

		// int s = search_symbol(no->name);
		// if(s == -1 || !tsimbolos[s].exists){
			// printf("%d.%d: error: símbolo %s não declarado.\n", esimbolos[error_count].linha + 1, esimbolos[error_count].coluna, no->name);
			// error_count++;
		// }
	}

	// check_division_zero(no);
}

void check_division_zero(noh **root, noh *no){
	if(no->type == DIVIDE && no->children[1]->type == INTEGER && no->children[1] && no->children[1]->intv == 0){
		printf("%d.%d: \t", yylineno - 1, yycol);
		error();
		printf(" divisão por zero.\n");
		error_count++;
	}
}

void check_type_incompatible(noh **root, noh *no){
	// if(no->type == DIVIDE && no->children[1]->type == INTEGER && no->children[1] && no->children[1]->intv == 0)
	// 	printf("%d.%d: \terror: divisão por zero.\n", yylineno - 1, pegar_coluna());
	switch (no->type)
	{
	case SUM:
	case MINUS:
	case DIVIDE:
	case MULTI:
	case POW:
		// if((no->children[0]->type == INTEGER && no->children[1]->type == FLOAT) || (no->children[1]->type == INTEGER && no->children[0]->type == FLOAT)){
		// 	printf("%d.%d: \terror: tipos incompativeis (%s %s %s).\n", yylineno - 1, yycol, no->children[0]->type, no->children[0]->type, no->children[0]->type);
		// 	error_count++;
		// }
		if((no->children[0]->type == INTEGER && no->children[1]->type == FLOAT) || (no->children[1]->type == INTEGER && no->children[0]->type == FLOAT)){
			printf("%d.%d: \t", yylineno - 1, yycol);
			error();
			printf(" tipos incompativeis (%s %s %s).\n", noh_type_names[no->children[0]->type], noh_type_names[no->type], noh_type_names[no->children[1]->type]);
			error_count++;
		}
		if(no->children[1]){
			if(no->children[0]->name && no->children[1]->name && simbolo_existe(no->children[0]->name) && simbolo_existe(no->children[1]->name)){
				simbolo *s1 = buscar_simbolo(no->children[0]), *s2 = buscar_simbolo(no->children[1]);
				if(s1 != s2){
					printf("%d.%d: \t1", yylineno - 1, yycol);
					error();
					printf(" tipos incompativeis (%s %s %s).\n", s2->tipo, noh_type_names[no->type], s1->tipo);
				error_count++;
				}
			}else if(no->children[0]->name && simbolo_existe(no->children[0]->name) && 
			(strcmp(buscar_simbolo(no->children[0])->tipo, "int") == 0|| strcmp(buscar_simbolo(no->children[0])->tipo, "float") == 0) && 
			(strcmp(noh_type_names[no->children[1]->type], "int") == 0|| strcmp(noh_type_names[no->children[1]->type], "float") == 0)){
				simbolo *s1 = buscar_simbolo(no->children[0]);
				if(strcmp(s1->tipo, noh_type_names[no->children[1]->type]) != 0){
					printf("%d.%d: \t2", yylineno - 1, yycol);
					error();
					printf(" tipos incompativeis (%s %s %s).\n", s1->tipo, noh_type_names[no->type], noh_type_names[no->children[1]->type]);
					// printf("nome %s nome %s\n", buscar_simbolo(no->children[0])->nome, no->children[1]->name);
				error_count++;
				}
			}else if(no->children[1]->name && simbolo_existe(no->children[1]->name) && 
			(strcmp(buscar_simbolo(no->children[1])->tipo, "int") == 0|| strcmp(buscar_simbolo(no->children[1])->tipo, "float") == 0) && 
			(strcmp(noh_type_names[no->children[0]->type], "int") == 0|| strcmp(noh_type_names[no->children[0]->type], "float") == 0)){
				simbolo *s2 = buscar_simbolo(no->children[1]);
				if(strcmp(s2->tipo, noh_type_names[no->children[0]->type]) != 0){
					printf("%d.%d: \t3", yylineno - 1, yycol);
					error();
					printf(" tipos incompativeis (%s %s %s).\n", noh_type_names[no->children[0]->type], noh_type_names[no->type], s2->tipo);
					// printf("nome %s nome %s\n", buscar_simbolo(no->children[0])->nome, no->children[1]->name);
				error_count++;
				}
			}
			// else{
			// 	simbolo *s2 = buscar_simbolo(no->children[1]);
			// 	if(strcmp(s2->tipo, noh_type_names[no->children[0]->type])){
			// 		printf("%d.%d: \t1error: tipos incompativeis (%s %s %s).\n", yylineno - 1, yycol, s2->tipo , noh_type_names[no->type], noh_type_names[no->children[0]->type]);
			// 	error_count++;
			// 	}
			// }

			// if(no->children[0]->name && simbolo_existe(no->children[0]->name) && strcmp(buscar_simbolo(no->children[0])->tipo, noh_type_names[no->children[1]->type])){
			// 	printf("%d.%d: \t2error: tipos incompativeis (%s %s %s).\n", yylineno - 1, yycol, buscar_simbolo(no->children[0])->tipo, noh_type_names[no->type], noh_type_names[no->children[1]->type]);
			// 	error_count++;
			// }
		}
		break;
	}
}

void check_print(noh **root, noh *no){
	// printf("%d <<<<<<<<<\n", no->type);
	if(no->type == TRUE || no->type == FALSE){
		printf("%d.%d: \t", yylineno - 1, yycol);
		error();
		printf(" não pode printar booleano.\n");
		error_count++;
	}
}

simbolo *buscar_simbolo(noh *no){
	// printf("NOME %s\n", no->name);
	for(int i = 0; i < simbolo_qnt; i++){
			// printf("NOME2 %s\n", tsimbolos[i].nome);
		if(strcmp(no->name, tsimbolos[i].nome) == 0){
			return &tsimbolos[i];
		}
	}
}

// void check_division_zero(noh *no){
// 	// printf("teste %d\n", no->id);
// 	// if(no->intv == 0 && no->type == INTEGER)
// 	if(no->type == DIVIDE && no->children[1]->type == INTEGER && no->children[1]->intv == 0 && no->children[1])
// 		printf("%d.%d: error: divisão por zero.\n", yylineno - 1, pegar_coluna());
// }

void visitor_leaf_first(noh **root, visitor_action act){
	noh *r = *root;
	for(int i = 0; i < r->childcount; i++){
		visitor_leaf_first(&r->children[i], act);
		if(act)
			act(root, r->children[i]);
	}
}



simbolo *simbolo_novo(char *nome, int token, int tipo){
// simbolo *simbolo_novo(char *nome, int token){
	tsimbolos[simbolo_qnt].nome = nome;
	tsimbolos[simbolo_qnt].token = token;
	tsimbolos[simbolo_qnt].exists = false;
	if(tipo == INT)
		tsimbolos[simbolo_qnt].tipo = "int";
	else
		tsimbolos[simbolo_qnt].tipo = "float";
	// printf("tipo %s\n", tsimbolos[simbolo_qnt].tipo);
	simbolo *result = &tsimbolos[simbolo_qnt];
	simbolo_qnt++;
	linha_coluna(result);
	// printf(">entrou %d\n", simbolo_qnt);
	return result;
}

simbolo *simbolo_erro(char *nome, int token, int n){
	// esimbolos[simbolo_qnt_erro].nome = nome;
	// esimbolos[simbolo_qnt_erro].token = token;
	// esimbolos[simbolo_qnt_erro].exists = false;
	simbolo *result = &esimbolos[simbolo_qnt_erro];
	// simbolo_qnt_erro++;
	// linha_coluna(result);
	printf("%d.%d: \t", yylineno - n, yycol);
	error();
	printf(" símbolo %s não declarado.\n", nome);
	error_count++;
	// printf(">entrou %d\n", simbolo_qnt);
	return result;
}

bool simbolo_existe(char *nome){
	// busca linear, nao eficiente
	// printf(">entrou2 %d %s\n", simbolo_qnt, nome);
	for(int i = 0; i < simbolo_qnt; i++){
		if(strcmp(tsimbolos[i].nome, nome) == 0)
			return true;
	}
	return false;
}

void debug(){
	printf("Simbolos: ");
	// printf(">> %d \n", simbolo_qnt);
	for(int i = 0; i < simbolo_qnt; i++)
		printf("%d{%s}%s ", tsimbolos[i].linha, tsimbolos[i].nome, tsimbolos[i].tipo);
	printf("\nErros: %d\n", error_count);
}

void error(){
	printf("\033[0;31m");
	printf("error:");
	printf("\033[0m");
}