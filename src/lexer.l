%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void yyerror(const char *s) {
    fprintf(stderr, "Erro: %s\n", s);
}

%}

%option noyywrap

%%

"def"                { printf("TOKEN: 'def' - Palavra-chave\n"); }
"return"             { printf("TOKEN: 'return' - Palavra-chave\n"); }
"if"                 { printf("TOKEN: 'if' - Palavra-chave\n"); }
"else"               { printf("TOKEN: 'else' - Palavra-chave\n"); }
"while"              { printf("TOKEN: 'while' - Palavra-chave\n"); }
"for"                { printf("TOKEN: 'for' - Palavra-chave\n"); }
"in"                 { printf("TOKEN: 'in' - Palavra-chave\n"); }
"range"              { printf("TOKEN: 'range' - Palavra-chave\n"); }
"True"               { printf("TOKEN: 'True' - Literal booleano\n"); }
"False"              { printf("TOKEN: 'False' - Literal booleano\n"); }

[0-9]+               { printf("TOKEN: '%s' - Literal inteiro\n", yytext); }
[0-9]+"."[0-9]+     { printf("TOKEN: '%s' - Literal flutuante\n", yytext); }

[A-Za-z_][A-Za-z0-9_]* { printf("TOKEN: '%s' - Identificador\n", yytext); }

"+"                   { printf("TOKEN: '+' - Operador\n"); }
"-"                   { printf("TOKEN: '-' - Operador\n"); }
"*"                   { printf("TOKEN: '*' - Operador\n"); }
"/"                   { printf("TOKEN: '/' - Operador\n"); }
"%"                   { printf("TOKEN: '%%' - Operador\n"); }
"=="                  { printf("TOKEN: '==' - Operador de comparação\n"); }
"!="                  { printf("TOKEN: '!=' - Operador de comparação\n"); }
"<"                   { printf("TOKEN: '<' - Operador de comparação\n"); }
">"                   { printf("TOKEN: '>' - Operador de comparação\n"); }
"<="                  { printf("TOKEN: '<=' - Operador de comparação\n"); }
">="                  { printf("TOKEN: '>=' - Operador de comparação\n"); }
"="                   { printf("TOKEN: '=' - Atribuição\n"); }

"and"                 { printf("TOKEN: 'and' - Operador lógico\n"); }
"or"                  { printf("TOKEN: 'or' - Operador lógico\n"); }
"not"                 { printf("TOKEN: 'not' - Operador lógico\n"); }

"("                   { printf("TOKEN: '(' - Delimitador\n"); }
")"                   { printf("TOKEN: ')' - Delimitador\n"); }
"{"                   { printf("TOKEN: '{' - Delimitador\n"); }
"}"                   { printf("TOKEN: '}' - Delimitador\n"); }
","                   { printf("TOKEN: ',' - Delimitador\n"); }
":"                   { printf("TOKEN: ':' - Delimitador\n"); }
";"                   { printf("TOKEN: ';' - Delimitador\n"); }

[ \t\n\r]+            { /* Ignorar espaços em branco, tabs e quebras de linha */ }

.                     { printf("TOKEN: '%s' - Caractere desconhecido\n", yytext); }

%%

int main() {
    yylex();  
    return 0;
}
