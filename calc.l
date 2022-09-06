%{
	#include <stdio.h>
	#include "calc.tab.h"
%}

%option yylineno

N	[0-9]
L	[a-zA-Z_]

%%

"print"			{ return TOK_PRINTA; }

"="				{ return '='; }
"*"				{ return '*'; }
"+"				{ return '+'; }
"-"				{ return '-'; }
"/"				{ return '/'; }
"("				{ return '('; }
")"				{ return ')'; }

[ \t\n\r]       {  }
[0-9]+			{ return TOK_INTEIRO; }
{N}+\.{N}+		{ return TOK_REAL; }
{L}({L}|{N})*	{ return TOK_LETRA; }

.				{ printf("TOK_ESSE_SIMBOLO_NAO_PODE\n"); }

%%

int yywrap(){
    return 1;
}

int main(int argc, char *argv[]){
	if(argc <= 1){
		printf("%s [file]\n", argv[0]);
		return 1;
	}
	
	FILE *f = fopen(argv[1], "r");
	//passar o arquivo que quero que compila
	yyin = f;
	//compila
	yyparse();
	fclose(f);
	
	return 0;
}
