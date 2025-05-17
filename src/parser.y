%{ 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tabela.h"
#include "ast.h"

FILE *output;
extern FILE *yyin;
NoAST *raiz = NULL; // Raiz da AST

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
	NoAST* ast;    // Ponteiro para nós da AST
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

%type <string> function_stmt statements statement  // Mantemos para compatibilidade com o código existente

%type <var> variable_declaration value       // Mantemos para compatibilidade com o código existente

%type <intValue> expr                       // Mantemos para compatibilidade com o código existente

// Novos tipos para a AST
%type <ast> ast_program ast_function_stmt ast_statements ast_statement ast_variable_declaration ast_value ast_expr

%%

program:
	statements |
	function_stmts
	;

// Regras para construção da AST (executadas em paralelo com as regras tradicionais)
ast_program:
	ast_statements { 
		raiz = $1; 
		$$ = raiz;
	} |
	ast_function_stmt { 
		raiz = $1; 
		$$ = raiz;
	}
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
		
		// Criar o nó AST para a função
		NoAST* ast_node = criarNoFuncao("void", $2, NULL, NULL);
		
		// Adicionar à raiz da AST se não existir
		if (raiz == NULL) {
			raiz = ast_node;
		} else {
			adicionarIrmao(raiz, ast_node);
		}
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

// Regra AST para statements
ast_statements:
	ast_statements ast_statement {
		$$ = adicionarIrmao($1, $2);
	}
	| ast_statement {
		$$ = $1;
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

// Regra AST para statement
ast_statement:
	ast_variable_declaration {
		$$ = $1;
	} |
	RETURN {
		// Nó para return sem valor
		$$ = criarNoOp('r', NULL, NULL); // Usando 'r' como operador para return
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

// Versão AST para declaração de variável
ast_variable_declaration:
		ID ASSIGN ast_value	{ 
			// Criar nós AST
			NoAST *id_node = criarNoId($1);
			$$ = criarNoAtribuicao(id_node, $3);
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

// Versão AST para valores
ast_value:
    TRUE {
        $$ = criarNoNum(1);  // TRUE como 1
    }
    | FALSE {
        $$ = criarNoNum(0);  // FALSE como 0
    }
    | ast_expr {
        $$ = $1;
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
    
// Versão AST para expressões
ast_expr:
    ast_expr PLUS ast_expr {
        $$ = criarNoOp('+', $1, $3);
    }
    | ast_expr MINUS ast_expr {
        $$ = criarNoOp('-', $1, $3);
    }
    | ast_expr TIMES ast_expr {
        $$ = criarNoOp('*', $1, $3);
    }
    | ast_expr DIVIDE ast_expr {
        $$ = criarNoOp('/', $1, $3);
    }
    | LPAREN ast_expr RPAREN {
        $$ = $2;
    }
    | NUM {
        $$ = criarNoNum(atoi($1));
    }
    | ID {
        $$ = criarNoId($1);
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
	
	// Processar a AST gerada
	if (raiz != NULL) {
		fprintf(stdout, "AST gerada com sucesso!\n");
		imprimirASTDetalhada(raiz, 0);
	} else {
		fprintf(stderr, "Aviso: AST não foi gerada completamente\n");
	}
	
	fclose(input);
	fclose(output);
	imprimirTabela();
	
	// Liberar a memória utilizada pela AST
	if (raiz != NULL) {
		liberarAST(raiz);
	}
	
    return 0;
}
