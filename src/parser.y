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
%type <string> param_list param while_stmt for_stmt condition function_call

%type <var> variable_declaration value       // Mantemos para compatibilidade com o código existente

%type <intValue> expr expr_list arg_list     // Mantemos para compatibilidade com o código existente

// Novos tipos para a AST
%type <ast> ast_function_stmts ast_function_stmt ast_statements ast_statement 
%type <ast> ast_variable_declaration ast_value ast_expr
%type <ast> ast_param_list ast_param ast_function_call ast_expr_list ast_arg_list
%type <ast> ast_while_stmt ast_for_stmt ast_condition ast_block

%%

program:
	ast_statements { 
		raiz = $1; 
	} |
	ast_function_stmts { 
		raiz = $1; 
	}
	;

ast_function_stmts:
	ast_function_stmts ast_function_stmt {
		$$ = adicionarIrmao($1, $2);
	} |
	ast_function_stmt {
		$$ = $1;
	}
	;

function_stmt:
	DEF ID LPAREN param_list RPAREN COLON INDENT statements DEDENT {
		// Adicionar função na tabela de símbolos
		inserirSimbolo($2, "function");
		fprintf(output, "void %s(%s) {\n%s\n}\n", $2, $4 ? $4 : "", $8);
		
		// Criar o nó AST para a função - não conectado com a AST real ainda
	} |
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

// Regra AST para declaração de função
ast_function_stmt:
	DEF ID LPAREN ast_param_list RPAREN COLON INDENT ast_statements DEDENT {
		// Criar nó de função com parâmetros
		$$ = criarNoFuncao("void", $2, $4, $8);
	} |
	DEF ID LPAREN RPAREN COLON INDENT ast_statements DEDENT {
		// Criar nó de função sem parâmetros
		$$ = criarNoFuncao("void", $2, NULL, $7);
	}
	;

param_list:
	param_list COMMA param {
		char *result = malloc(strlen($1) + strlen($3) + 3);
		sprintf(result, "%s, %s", $1, $3);
		$$ = result;
	} |
	param {
		$$ = $1;
	} |
	/* vazio */ {
		$$ = strdup("");
	}
	;

// Regra AST para lista de parâmetros
ast_param_list:
	ast_param_list COMMA ast_param {
		$$ = adicionarIrmao($1, $3);
	} |
	ast_param {
		$$ = $1;
	} |
	/* vazio */ {
		$$ = NULL;
	}
	;

param:
	ID COLON TYPE_INT {
		inserirSimbolo($1, "int");
		$$ = strdup("int ");
		strcat($$, $1);
	} |
	ID COLON TYPE_FLOAT {
		inserirSimbolo($1, "float");
		$$ = strdup("float ");
		strcat($$, $1);
	} |
	ID COLON TYPE_BOOL {
		inserirSimbolo($1, "bool");
		$$ = strdup("bool ");
		strcat($$, $1);
	} |
	ID {
		inserirSimbolo($1, "auto");
		$$ = strdup("auto ");
		strcat($$, $1);
	}
	;

// Regra AST para parâmetro
ast_param:
	ID COLON TYPE_INT {
		inserirSimbolo($1, "int");
		$$ = criarNoDeclaracao("int", $1, NULL);
	} |
	ID COLON TYPE_FLOAT {
		inserirSimbolo($1, "float");
		$$ = criarNoDeclaracao("float", $1, NULL);
	} |
	ID COLON TYPE_BOOL {
		inserirSimbolo($1, "bool");
		$$ = criarNoDeclaracao("bool", $1, NULL);
	} |
	ID {
		inserirSimbolo($1, "auto");
		$$ = criarNoDeclaracao("auto", $1, NULL);
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
	} |
	while_stmt {
		$$ = $1;
	} |
	for_stmt {
		$$ = $1;
	} |
	function_call {
		$$ = strdup("");
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
	} |
	RETURN ast_expr {
		// Nó para return com um valor
		$$ = criarNoOp('r', $2, NULL); // Usando 'r' como operador para return com valor
	} |
	ast_while_stmt {
		$$ = $1;
	} |
	ast_for_stmt {
		$$ = $1;
	} |
	ast_function_call {
		$$ = $1;
	}
	;

// Nova regra para while
while_stmt:
	WHILE condition COLON INDENT statements DEDENT {
		// Gerar código C para while
		fprintf(output, "while (%s) {\n%s\n}\n", $2, $5);
		$$ = strdup("");
	}
	;

// Regra AST para while
ast_while_stmt:
	WHILE ast_condition COLON INDENT ast_statements DEDENT {
		// Criar nó para o laço while
		$$ = criarNoWhile($2, $5);
	}
	;

// Nova regra para for-in-range
for_stmt:
	FOR ID IN RANGE LPAREN expr RPAREN COLON INDENT statements DEDENT {
		// Gerar código C para o for
		fprintf(output, "for (int %s = 0; %s < %s; %s++) {\n%s\n}\n", 
				$2, $2, $6, $2, $10);
		$$ = strdup("");
	} |
	FOR ID IN RANGE LPAREN expr COMMA expr RPAREN COLON INDENT statements DEDENT {
		// For com início e fim especificados
		fprintf(output, "for (int %s = %s; %s < %s; %s++) {\n%s\n}\n", 
				$2, $6, $2, $8, $2, $12);
		$$ = strdup("");
	}
	;

// Regra AST para for-in-range
ast_for_stmt:
	FOR ID IN RANGE LPAREN ast_expr RPAREN COLON INDENT ast_statements DEDENT {
		// Criar nós para um for simples (0 até <expr>)
		NoAST *id_node = criarNoId($2);
		NoAST *inicio = criarNoNum(0);
		NoAST *atrib_init = criarNoAtribuicao(id_node, inicio);
		
		NoAST *id_cond = criarNoId($2);
		NoAST *cond = criarNoOp('<', id_cond, $6);
		
		NoAST *id_incr = criarNoId($2);
		NoAST *um = criarNoNum(1);
		NoAST *incr = criarNoOp('+', id_incr, um);
		NoAST *atrib_incr = criarNoAtribuicao(criarNoId($2), incr);
		
		// Criar bloco para for (inicialização, condição, incremento, corpo)
		NoAST *bloco = criarNoBloco($10);
		
		// Implementar como um "while" especial com inicialização e incremento
		NoAST *corpo_completo = adicionarIrmao($10, atrib_incr);
		
		// Simular um "for" usando um bloco que contém a inicialização e um while
		NoAST *while_node = criarNoWhile(cond, corpo_completo);
		$$ = adicionarIrmao(atrib_init, while_node);
	}
	;

// Nova regra para condição
condition:
	expr {
		$$ = $1;
	}
	;

// Regra AST para condição
ast_condition:
	ast_expr {
		$$ = $1;
	}
	;

// Nova regra para bloco de código
ast_block:
	INDENT ast_statements DEDENT {
		$$ = criarNoBloco($2);
	}
	;

// Nova regra para chamada de função
function_call:
	ID LPAREN arg_list RPAREN {
		// Verificar se a função existe
		Simbolo *s = buscarSimbolo($1);
		if (!s) {
			fprintf(stderr, "Erro semântico: Função '%s' não declarada\n", $1);
		}
		
		// Gerar código para chamada de função
		fprintf(output, "%s(%s);\n", $1, $3 ? $3 : "");
		$$ = strdup("");
	}
	;

// Regra AST para chamada de função
ast_function_call:
	ID LPAREN ast_arg_list RPAREN {
		// Criar nó para chamada de função
		$$ = criarNoChamada($1, $3);
	}
	;

// Nova regra para lista de argumentos
arg_list:
	expr_list {
		$$ = $1;
	} |
	/* vazio */ {
		$$ = strdup("");
	}
	;

// Regra AST para lista de argumentos
ast_arg_list:
	ast_expr_list {
		$$ = $1;
	} |
	/* vazio */ {
		$$ = NULL;
	}
	;

// Nova regra para lista de expressões
expr_list:
	expr_list COMMA expr {
		char *result = malloc(strlen($1) + strlen($3) + 3);
		sprintf(result, "%s, %s", $1, $3);
		$$ = result;
	} |
	expr {
		$$ = $1;
	}
	;

// Regra AST para lista de expressões
ast_expr_list:
	ast_expr_list COMMA ast_expr {
		$$ = adicionarIrmao($1, $3);
	} |
	ast_expr {
		$$ = $1;
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
    | expr LT expr      { $$ = strdup("expr < expr"); }
    | expr GT expr      { $$ = strdup("expr > expr"); }
    | expr LE expr      { $$ = strdup("expr <= expr"); }
    | expr GE expr      { $$ = strdup("expr >= expr"); }
    | expr EQ expr      { $$ = strdup("expr == expr"); }
    | expr NE expr      { $$ = strdup("expr != expr"); }
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
    | function_call     { $$ = strdup("function_call"); }
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
    | ast_expr LT ast_expr {
        $$ = criarNoOp('<', $1, $3);
    }
    | ast_expr GT ast_expr {
        $$ = criarNoOp('>', $1, $3);
    }
    | ast_expr LE ast_expr {
        $$ = criarNoOp('L', $1, $3); // L para '≤'
    }
    | ast_expr GE ast_expr {
        $$ = criarNoOp('G', $1, $3); // G para '≥'
    }
    | ast_expr EQ ast_expr {
        $$ = criarNoOp('=', $1, $3); // = para '=='
    }
    | ast_expr NE ast_expr {
        $$ = criarNoOp('!', $1, $3); // ! para '!='
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
    | ast_function_call {
        $$ = $1;
    }
    ;

%%

// Melhorando o sistema de mensagens de erro no parser
void yyerror(const char *s) {
    extern int yylineno;
    fprintf(stderr, "Erro de sintaxe na linha %d: %s\n", yylineno, s);
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
		fprintf(stdout, "Gerando código C a partir da AST...\n");
		
		// Imprimir a AST para fins de debug
		imprimirASTDetalhada(raiz, 0);
		
		// Gerar código C a partir da AST
		gerarCodigoC(raiz, output);
		
		fprintf(stdout, "Código C gerado com sucesso!\n");
	} else {
		fprintf(stderr, "Erro: AST não foi gerada\n");
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
