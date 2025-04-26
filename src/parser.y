%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

FILE *output;
extern FILE *yyin;

extern int yylex();
void yyerror(const char *s);

%}

%union {
    char* id;
    int ival;
    float fval;
}

%token DEF RETURN IF ELSE WHILE FOR IN RANGE TRUE FALSE
%token <ival> INT
%token <fval> FLOAT
%token <id> ID
%token EQ NE LT GT LE GE ASSIGN
%token AND OR NOT
%token PLUS MINUS TIMES DIVIDE MOD
%token LPAREN RPAREN LBRACE RBRACE COMMA COLON SEMICOLON

%type <id> function_stmt suite statement expression


%%

program:
    function_stmt {
        printf("Program parsed successfully.\n");
    }
    ;

function_stmt:
    DEF ID LPAREN RPAREN COLON {
    	fprintf(output, "void %s (){\n", $2);
    }
    ;

suite:
    statement {
        printf("Suite parsed.\n");
    }
    ;

statement:
    RETURN expression {
        $$ = strdup("");
        printf("Return statement parsed: return %s;\n", $2);
    }
    | expression {
        printf("Expression statement parsed: %s;\n", $1);
    }
    ;

expression:
    INT        { asprintf(&$$, "%d", $1); printf("Integer parsed: %d\n", $1); }
    | FLOAT    { asprintf(&$$, "%f", $1); printf("Float parsed: %f\n", $1); }
    | ID       { asprintf(&$$, "%s", $1); printf("Identifier parsed: %s\n", $1); }
    | expression PLUS expression {
        asprintf(&$$, "%s + %s", $1, $3);
        printf("Addition parsed: %s + %s\n", $1, $3);
    }
    | expression MINUS expression {
        asprintf(&$$, "%s - %s", $1, $3);
        printf("Subtraction parsed: %s - %s\n", $1, $3);
    }
    | expression TIMES expression {
        asprintf(&$$, "%s * %s", $1, $3);
        printf("Multiplication parsed: %s * %s\n", $1, $3);
    }
    | expression DIVIDE expression {
        asprintf(&$$, "%s / %s", $1, $3);
        printf("Division parsed: %s / %s\n", $1, $3);
    }
    ;

%%

void yyerror(const char *s) {
    fprintf(stderr, "Erro de sintaxe: %s\n", s);
}

int main(int arc, char **argv) {

	if (arc < 2) {
		fprintf(stderr, "Uso: %s <arquivo_de_entrada>\n", argv[0]);
		return 1;
	}

	FILE *input = fopen(argv[1], "r");
	if (input == NULL) {
		fprintf(stderr, "Erro ao abrir o arquivo de entrada: %s\n", argv[1]);
		return 1;
		}

		output = fopen("output.c", "w");
		if (output == NULL) {
				fprintf(stderr, "Erro ao abrir o arquivo de saída.\n");
				fclose(input);
				return 1;
		}

		yyin = input;

		yyparse();
		fclose(input);
		fclose(output);
    return 0;
}
