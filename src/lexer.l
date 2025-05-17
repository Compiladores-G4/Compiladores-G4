
%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.tab.h"  // Incluindo os tokens gerados pelo Bison

int indent_level = 0;
int current_line_indent = 0;

%}

%x STARTLINE

%%


\n { current_line_indent = 0; BEGIN(STARTLINE); printf("startline\n"); }
<STARTLINE>[ \t]+ { printf("TAB"); current_line_indent++; }
<STARTLINE>\n { printf("QUEBRADELINHA\n"); current_line_indent = 0; }
<STARTLINE>. {
	 unput(*yytext);
	 if (current_line_indent > indent_level) {
		 indent_level++;
			printf("INDENT\n");
		 return INDENT;
	 } else if (current_line_indent < indent_level) {
		 indent_level--;
			printf("DEDENT\n");
		 return DEDENT;
	 } else {
		 printf("INITIAL\n");
		 BEGIN (INITIAL);
	 }
}
<<EOF>> {
	printf(("EOF\n"));
	if (indent_level > 0) {
		indent_level--;
		return DEDENT;
	}
	return 0;
}

"def"                { return DEF; }
"return"             { return RETURN; }
"if"                 { return IF; }
"else"               { return ELSE; }
"while"              { return WHILE; }
"for"                { return FOR; }
"in"                 { return IN; }
"range"              { return RANGE; }
"True"               { return TRUE; }
"False"              { return FALSE; }

"int" 							 { return TYPE_INT; }
"float" 						 { return TYPE_FLOAT; }
"bool" 							 { return TYPE_BOOL; }

[0-9]+               { yylval.string = strdup(yytext); return NUM; }
[0-9]+"."[0-9]+      { yylval.string = strdup(yytext); return NUM; }

[A-Za-z_][A-Za-z0-9_]* {
    yylval.string = strdup(yytext);
    return ID;
}

"=="                  { return EQ; }
"!="                  { return NE; }
"<"                   { return LT; }
">"                   { return GT; }
"<="                  { return LE; }
">="                  { return GE; }
"="                   { return ASSIGN; }

"->"									{ return ARROW; }

"and"                 { return AND; }
"or"                  { return OR; }
"not"                 { return NOT; }

"+"                   { return PLUS; }
"-"                   { return MINUS; }
"*"                   { return TIMES; }
"/"                   { return DIVIDE; }
"%"                   { return MOD; }

"("                   { return LPAREN; }
")"                   { return RPAREN; }
"{"                   { return LBRACE; }
"}"                   { return RBRACE; }
","                   { return COMMA; }
":"                   { return COLON; }
";"                   { return SEMICOLON; }

[ ]+            { /* Ignorar espaços em branco, tabs e quebras de linha */ }

%%
