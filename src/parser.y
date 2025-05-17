%{ 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tabela.h"

FILE *output;
extern FILE *yyin;

extern int yylex();
void yyerror(const char *s);

// Função para determinar o tipo com base no valor
char* determinarTipo(char *valor) {
    if (valor == NULL) return "desconhecido";
    if (strcmp(valor, "TRUE") == 0 || strcmp(valor, "FALSE") == 0) return "bool";
    // Verificar se é um número inteiro ou de ponto flutuante
    char *endptr;
    strtol(valor, &endptr, 10);
    if (*endptr == '\0') return "int";
    strtod(valor, &endptr);
    if (*endptr == '\0') return "float";
    return "desconhecido";
}

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
	char* intValue; // Adicionado para expressões
}

%token DEF RETURN IF ELSE WHILE FOR IN RANGE
%token <string> NUM TRUE FALSE
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

%type <intValue> expr

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
		// Adicionar função na tabela de símbolos
		inserirSimbolo($2, "function");
		fprintf(output, "void %s() {\n%s\n}\n", $2, $7);
	}
	;
statements:
	statements statement {
		// Concatenar os statements
		char *result = malloc(strlen($1) + strlen($2) + 2);
		sprintf(result, "%s\n%s", $1, $2);
		$$ = result;
	}
	| statement {
		$$ = $1 ? $1 : strdup("");
	}
	;

statement:
	variable_declaration { 
		$$ = strdup(""); // O código já foi gerado na regra variable_declaration
	} |
	RETURN { 
		fprintf(output, "return;\n");
		$$ = strdup("return;");
	}
	;

variable_declaration:
		ID ASSIGN value	{ 
			// Detectar o tipo da variável e adicioná-la à tabela de símbolos
			char *tipo = $3.type ? $3.type : "desconhecido";
			inserirSimbolo($1, tipo);
			
			// Preparar a variável de retorno para uso em outras regras
			$$.name = $1;
			$$.type = tipo;
			$$.value = $3.value;
			
			// Gerar código
			fprintf(output, "%s %s = %s;\n", tipo, $1, $3.value ? $3.value : "");
		}
	;
	
value:
    TRUE     { 
        $$.value = "1"; 
        $$.type = "bool";
    }
    | FALSE    { 
        $$.value = "0"; 
        $$.type = "bool";
    }
    | expr     {
        $$.value = $1;
        $$.type = "int"; // Assumindo que expressões são inteiras por padrão
    }
    ;
	
expr:
      expr PLUS expr    { $$ = strdup("expr + expr"); }
    | expr MINUS expr   { $$ = strdup("expr - expr"); }
    | expr TIMES expr   { $$ = strdup("expr * expr"); }
    | expr DIVIDE expr  { $$ = strdup("expr / expr"); }
    | LPAREN expr RPAREN{ $$ = $2; }
    | NUM               { $$ = $1; }
    | ID                { 
        // Verificar se a variável existe na tabela de símbolos
        Simbolo *s = buscarSimbolo($1);
        if (!s) {
            fprintf(stderr, "Erro semântico: Variável '%s' não declarada\n", $1);
        }
        $$ = $1;
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
		imprimirTabela();
    return 0;
}
