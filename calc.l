%{
	#include <stdio.h>
	#include "header.h"
	#include "calc.tab.h"
%}

%option yylineno

N	[0-9]
L	[a-zA-Z_]

%%

"print"			{ printf("%s\n", strndup(yytext, yyleng)); return TOK_PRINT; }
"if"			{ printf("%s\n", strndup(yytext, yyleng)); return TOK_IF; }
"else"			{ printf("%s\n", strndup(yytext, yyleng)); return TOK_ELSE; }
"while"			{ printf("%s\n", strndup(yytext, yyleng)); return TOK_WHILE; }
"else if"		{ printf("%s\n", strndup(yytext, yyleng)); return TOK_ELSEIF; }

"true" 			{ yylval.args.ident = strndup(yytext, yyleng); return TOK_TRUE; }
"false"			{ yylval.args.ident = strndup(yytext, yyleng); return TOK_FALSE; }

"="				{ return '='; }
"*"				{ return '*'; }
"+"				{ return '+'; }
"-"				{ return '-'; }
"/"				{ return '/'; }
"("				{ return '('; }
")"				{ return ')'; }
"^"				{ return '^'; }
"{"				{ return '{'; }
"}"				{ return '}'; }
">" 			{ return '>'; }
"<" 			{ return '<'; }
"!"				{ return '!'; }
"&"				{ return '&'; }
"|"				{ return '|'; }

[ \t\n\r]       {  }
[0-9]+			{ yylval.args.intv = atoi(yytext); return TOK_INTEGER; }
{N}+\.{N}+		{ yylval.args.dblv = atof(yytext); return TOK_FLOAT; }
{L}({L}|{N})*	{ yylval.args.ident = strndup(yytext, yyleng); return TOK_IDENT; }

.				{ printf("%s TOK_ESSE_SIMBOLO_NAO_PODE\n", strndup(yytext, yyleng)); }

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
	//passar o arquivo alvo para compilar
	yyin = f;
	//compila
	yyparse();
	fclose(f);
	
	return 0;
}
