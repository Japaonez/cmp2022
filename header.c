//header.c
#include <stdlib.h>
#include <stdio.h>

#include "header.h"

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