%{
	#include <stdio.h>
%}

%option yylineno

N	[0-9]
L	[a-zA-Z_]

%%

"print"			{ printf("TOK_PRINTA\n"); }

"="				{ printf("TOK_IGUAL\n"); }
"*"				{ printf("TOK_MULTI\n"); }
"+"				{ printf("TOK_MAIS\n"); }
"-"				{ printf("TOK_MENOS\n"); }
"/"				{ printf("TOK_DIVIDIR\n"); }
"("				{ printf("TOK_ABREPAR\n"); }
")"				{ printf("TOK_FECHPAR\n"); }

[ \t\n\r]       {  }
[0-9]+			{ printf("TOK_INTEIRO<%s>\n", yytext); }
{N}+\.{N}+		{ printf("TOK_REAL<%s>\n", yytext); }
{L}({L}|{N})*	{ printf("TOK_LETRA<%s>\n", yytext); }

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
	yylex();
	fclose(f);
	
	return 0;
}
