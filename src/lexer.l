%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.tab.h"  // Incluindo os tokens gerados pelo Bison

%}

%option noyywrap

%%

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

[0-9]+               { yylval.ival = atoi(yytext); return INT; }
[0-9]+"."[0-9]+      { yylval.fval = atof(yytext); return FLOAT; }

[A-Za-z_][A-Za-z0-9_]* {
    yylval.id = strdup(yytext);
    return ID;
}

"=="                  { return EQ; }
"!="                  { return NE; }
"<"                   { return LT; }
">"                   { return GT; }
"<="                  { return LE; }
">="                  { return GE; }
"="                   { return ASSIGN; }

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

[ \t\n\r]+            { /* Ignorar espaços em branco, tabs e quebras de linha */ }

.                     { printf("Caractere desconhecido: %s\n", yytext); }

%%