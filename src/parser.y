%{ #include <stdio.h>
#include <stdlib.h>
#include <string.h>


FILE *output;
extern FILE *yyin;

extern int yylex();
void yyerror(const char *s);


%}

%code requires {
	typedef struct {
		char *value;
		char *type;
		char *name;
	} var_t;

}

%union {
	char* string;
	var_t var;
}

%token DEF RETURN IF ELSE WHILE FOR IN RANGE
%token <string> INT FLOAT TRUE FALSE
%token <string> ID
%token EQ NE LT GT LE GE ASSIGN
%token AND OR NOT
%token PLUS MINUS TIMES DIVIDE MOD
%token LPAREN RPAREN LBRACE RBRACE COMMA COLON SEMICOLON
%token INDENT DEDENT
%token ARROW
%token TYPE_INT TYPE_FLOAT TYPE_BOOL

%type <string> function_stmt statements statement 

%type <var> variable_declaration value


%%

program:
	statements |
	function_stmts
	;

function_stmts:
	function_stmts function_stmt |
	function_stmt
	;
function_stmt:
	DEF ID LPAREN RPAREN COLON INDENT statements DEDENT {
		fprintf(output, "void %s (){\n", $2);
		fprintf(output, "%s", $7);
		fprintf(output, "}\n", $2);
	}
	;
statements:
	statements statement |
	statement
	;

statement:
	variable_declaration { } |
	RETURN { $$ = "return;\n"; }
	;

variable_declaration:
		ID ASSIGN value	{ 
			fprintf(output, "%s %s = %s;\n", $3.type, $1, $3.value);
		}
	;
value:
	INT 		{ $$ = (var_t){ .value = yylval.string, .type = "int"}; 		} |
	FLOAT 	{ $$ = (var_t){ .value = yylval.string, .type = "float"}; 	}	|
	TRUE		{ $$ = (var_t){ .value = "1", .type = "int"}; } 							|
	FALSE		{ $$ = (var_t){ .value = "0", .type = "int"}; } 	
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
