%{ 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tipos.h"
#include "ast.h"
#include "tabela.h"

FILE *output;
extern FILE *yyin;

NoAST *raiz = NULL; 

extern int yylex();
void yyerror(const char *s);

extern char *yytext;
extern int yylineno;

// Contador de erros semânticos
int erros_semanticos = 0;

char* determinarTipo(char *valor) {
    if (valor == NULL) return "desconhecido";
    if (strcmp(valor, "TRUE") == 0 || strcmp(valor, "FALSE") == 0 || 
        strcmp(valor, "True") == 0 || strcmp(valor, "False") == 0) return "bool";
    char *endptr;
    strtol(valor, &endptr, 10);
    if (*endptr == '\0') return "int";
    strtod(valor, &endptr);
    if (*endptr == '\0') return "float";
    return "desconhecido";
}

// Inicializar a tabela de símbolos e escopos no início da análise
void inicializarCompilador() {
    inicializarEscopos();
    erros_semanticos = 0;
}

%}

%code requires {
#include "ast.h"
}

%union {
	char* string;
	int intValue; 
	float floatValue;
	NoAST* ast;
    char* tipo;
}

%token DEF RETURN IF ELSE ELIF WHILE FOR IN RANGE
%token <string> TRUE FALSE
%token <intValue> NUM
%token <string> FLOAT_NUM
%token <string> ID
%token EQ NE LT GT LE GE ASSIGN
%token AND OR NOT
%token PLUS MINUS TIMES DIVIDE MOD
%token LPAREN RPAREN LBRACE RBRACE LBRACKET RBRACKET COMMA COLON SEMICOLON
%token INDENT DEDENT
%token ARROW
%token <string> TYPE_INT TYPE_FLOAT TYPE_BOOL



// Definindo precedência dos operadores (menor para maior precedência)
%left OR
%left AND
%right NOT
%left EQ NE
%left LT GT LE GE
%left PLUS MINUS
%left TIMES DIVIDE MOD
%right UMINUS
%left LPAREN RPAREN

%type <ast> expr variable_declaration statement statements program conditional_stmt else_part function_stmt function_stmts while_stmt for_stmt parameter_list parameter list_expr element_list
%type <string> type_annotation

%%

program:
	{
        inicializarCompilador();
    }
	| statements {
		raiz = $1;
		$$ = raiz;
	}
	| function_stmts {
		raiz = $1;
		$$ = raiz;
	}
	;

function_stmts:
	function_stmts function_stmt {
		$$ = adicionarIrmao($1, $2);
	}
	| function_stmt {
		$$ = $1;
	}
	;

function_stmt:
	DEF ID LPAREN RPAREN COLON {
        // Inserir a função na tabela de símbolos do escopo atual (global)
        inserirFuncao($2, "void", 0);
        criarEscopo($2); // Cria um escopo para a função
    } INDENT statements DEDENT {
		$$ = criarNoFuncao("void", $2, NULL, $8);
        sairEscopo(); // Sai do escopo da função quando terminar
	}
	| DEF ID LPAREN parameter_list RPAREN COLON {
        // Contar parâmetros
        int numParams = 0;
        NoAST *param = $4;
        while (param) {
            numParams++;
            param = param->proximoIrmao;
        }
        // Inserir a função na tabela de símbolos do escopo atual (global)
        inserirFuncao($2, "void", numParams);
        criarEscopo($2); // Cria um escopo para a função
    } INDENT statements DEDENT {
		$$ = criarNoFuncao("void", $2, $4, $9);
        sairEscopo(); // Sai do escopo da função quando terminar
	}
	| DEF ID LPAREN RPAREN ARROW type_annotation COLON {
        // Inserir a função na tabela de símbolos do escopo atual (global)
        inserirFuncao($2, $6, 0);
        criarEscopo($2); // Cria um escopo para a função
    } INDENT statements DEDENT {
		$$ = criarNoFuncao($6, $2, NULL, $10);
        sairEscopo(); // Sai do escopo da função quando terminar
	}
	| DEF ID LPAREN parameter_list RPAREN ARROW type_annotation COLON {
        // Contar parâmetros
        int numParams = 0;
        NoAST *param = $4;
        while (param) {
            numParams++;
            param = param->proximoIrmao;
        }
        // Inserir a função na tabela de símbolos do escopo atual (global)
        inserirFuncao($2, $7, numParams);
        criarEscopo($2); // Cria um escopo para a função
    } INDENT statements DEDENT {
		$$ = criarNoFuncao($7, $2, $4, $11);
        sairEscopo(); // Sai do escopo da função quando terminar
	}
	;

parameter_list:
	parameter {
		$$ = $1;
	}
	| parameter_list COMMA parameter {
		$$ = adicionarIrmao($1, $3);
	}
	;

parameter:
	ID COLON type_annotation {
		$$ = criarNoDeclaracao($3, $1, NULL);
		// Inserir parâmetro na tabela de símbolos do escopo da função
		inserirSimbolo($1, $3);
	}
	;

type_annotation:
	TYPE_INT { $$ = $1; }
	| TYPE_FLOAT { $$ = $1; }
	| TYPE_BOOL { $$ = $1; }
	;

statements:
	statements statement {
		$$ = adicionarIrmao($1, $2);
	}
	| statement { 
		$$ = $1;	
	}
	;

statement:
	variable_declaration { $$ = $1; }
	| RETURN { $$ = criarNoOp('r', NULL, NULL); }
	| RETURN expr { 
		$$ = criarNoOp('r', $2, NULL); 
	}
	| conditional_stmt { $$ = $1; }
	| while_stmt { $$ = $1; }
	| for_stmt { $$ = $1; }
	| ID LPAREN RPAREN { $$ = criarNoChamada($1, NULL); }
	| ID LPAREN element_list RPAREN { $$ = criarNoChamada($1, $3); }
	;

while_stmt:
	WHILE expr COLON INDENT statements DEDENT {
		$$ = criarNoWhile($2, $5);
	}
	;

for_stmt:
	FOR ID IN RANGE LPAREN expr RPAREN COLON INDENT statements DEDENT {
		inserirSimbolo($2, "int");  // Declarar automaticamente a variável de iteração
		$$ = criarNoFor(criarNoId($2), criarNoNum(0), $6, criarNoNum(1), $10);
	}
	| FOR ID IN RANGE LPAREN expr COMMA expr RPAREN COLON INDENT statements DEDENT {
		inserirSimbolo($2, "int");  // Declarar automaticamente a variável de iteração
		$$ = criarNoFor(criarNoId($2), $6, $8, criarNoNum(1), $12);
	}
	| FOR ID IN RANGE LPAREN expr COMMA expr COMMA expr RPAREN COLON INDENT statements DEDENT {
		inserirSimbolo($2, "int");  // Declarar automaticamente a variável de iteração
		$$ = criarNoFor(criarNoId($2), $6, $8, $10, $14);
	}
	;

conditional_stmt:
	IF expr COLON INDENT statements DEDENT else_part {
		$$ = criarNoIf($2, $5, $7);
	}
	;

else_part:
	/* vazio */ { $$ = NULL; }
	| ELSE COLON INDENT statements DEDENT {
		$$ = $4;
	}
	| ELIF expr COLON INDENT statements DEDENT else_part {
		$$ = criarNoIf($2, $5, $7);
	}
	;

variable_declaration:
	ID ASSIGN expr { 
		NoAST *id_node = criarNoId($1);
		$$ = criarNoAtribuicao(id_node, $3);
		
		// Para atribuições com expressões, inferir o tipo
		char *tipo;
		if ($3->tipo == NO_ID) {
			// Se é atribuição de um ID, buscar o tipo desse ID
			Simbolo *s = buscarSimbolo($3->nome);
			if (s != NULL) {
				tipo = s->tipo;
			} else {
				tipo = "int"; // Assumir int como padrão para variáveis de loop
			}
		} else if ($3->tipo == NO_NUMERO) {
			tipo = "int";
		} else if ($3->tipo == NO_FLOAT) {
			tipo = "float";
		} else if ($3->tipo == NO_OPERADOR && $3->operador == 'f') {
			tipo = "float";
		} else if ($3->tipo == NO_OPERADOR && ($3->operador == 'T' || $3->operador == 'F')) {
			tipo = "bool";
		} else {
			tipo = "int"; // Assumir int como padrão
		}
		
		// Verificar se é uma redeclaração com tipo diferente
		Simbolo *s = buscarSimboloNoEscopo($1, obterNomeEscopoAtual());
		if (s != NULL && strcmp(s->tipo, "desconhecido") != 0 && 
            strcmp(s->tipo, tipo) != 0 && strcmp(tipo, "desconhecido") != 0) {
			printf("Erro semântico: redeclaração de '%s' com tipo incompatível\n", $1);
			erros_semanticos++;
		}
		
		// Inserir na tabela de símbolos
		inserirSimbolo($1, tipo);
	}
	;

expr:
	expr AND expr   			{ 
                                  $$ = criarNoOp('&', $1, $3); 
                                  if ($1->tipo == NO_ID && $3->tipo == NO_ID) {
                                      verificarOperacao($1->nome, $3->nome, '&');
                                  }
                                }
    | expr OR expr      			{ 
                                  $$ = criarNoOp('|', $1, $3); 
                                  if ($1->tipo == NO_ID && $3->tipo == NO_ID) {
                                      verificarOperacao($1->nome, $3->nome, '|');
                                  }
                                }
    | NOT expr          			{ 
                                  $$ = criarNoOp('~', $2, NULL); 
                                  if ($2->tipo == NO_ID) {
                                      // Verificar se o operando é válido para NOT
                                      Simbolo *s = buscarSimbolo($2->nome);
                                      if (s != NULL && strcmp(s->tipo, "bool") != 0 && strcmp(s->tipo, "desconhecido") != 0) {
                                          printf("Erro semântico: operador NOT requer operando booleano\n");
                                          erros_semanticos++;
                                      }
                                  }
                                }
	| expr PLUS expr    			{ 
                                  $$ = criarNoOp('+', $1, $3);
                                  // Se os nós forem identificadores, verificar compatibilidade
                                  if ($1->tipo == NO_ID && $3->tipo == NO_ID) {
                                      verificarOperacao($1->nome, $3->nome, '+');
                                  }
                                }
    | expr MINUS expr   			{ 
                                  $$ = criarNoOp('-', $1, $3); 
                                  if ($1->tipo == NO_ID && $3->tipo == NO_ID) {
                                      verificarOperacao($1->nome, $3->nome, '-');
                                  }
                                }
    | expr TIMES expr   			{ 
                                  $$ = criarNoOp('*', $1, $3); 
                                  if ($1->tipo == NO_ID && $3->tipo == NO_ID) {
                                      verificarOperacao($1->nome, $3->nome, '*');
                                  }
                                }
    | expr DIVIDE expr  			{ 
                                  $$ = criarNoOp('/', $1, $3); 
                                  if ($1->tipo == NO_ID && $3->tipo == NO_ID) {
                                      verificarOperacao($1->nome, $3->nome, '/');
                                  }
                                }
    | expr MOD expr     			{ 
                                  $$ = criarNoOp('%', $1, $3); 
                                  if ($1->tipo == NO_ID && $3->tipo == NO_ID) {
                                      verificarOperacao($1->nome, $3->nome, '%');
                                  }
                                }
    | expr EQ expr     			{ 
                                  $$ = criarNoOp('=', $1, $3); 
                                  if ($1->tipo == NO_ID && $3->tipo == NO_ID) {
                                      verificarOperacao($1->nome, $3->nome, '=');
                                  }
                                }
    | expr NE expr     			{ 
                                  $$ = criarNoOp('!', $1, $3); 
                                  if ($1->tipo == NO_ID && $3->tipo == NO_ID) {
                                      verificarOperacao($1->nome, $3->nome, '!');
                                  }
                                }
    | expr LT expr     			{ 
                                  $$ = criarNoOp('<', $1, $3); 
                                  if ($1->tipo == NO_ID && $3->tipo == NO_ID) {
                                      verificarOperacao($1->nome, $3->nome, '<');
                                  }
                                }
    | expr GT expr     			{ 
                                  $$ = criarNoOp('>', $1, $3); 
                                  if ($1->tipo == NO_ID && $3->tipo == NO_ID) {
                                      verificarOperacao($1->nome, $3->nome, '>');
                                  }
                                }
    | expr LE expr     			{ 
                                  $$ = criarNoOp('l', $1, $3); 
                                  if ($1->tipo == NO_ID && $3->tipo == NO_ID) {
                                      verificarOperacao($1->nome, $3->nome, 'l');
                                  }
                                }
    | expr GE expr     			{ 
                                  $$ = criarNoOp('g', $1, $3); 
                                  if ($1->tipo == NO_ID && $3->tipo == NO_ID) {
                                      verificarOperacao($1->nome, $3->nome, 'g');
                                  }
                                }
    | LPAREN expr RPAREN			{ $$ = $2; }
    | NUM               			{ $$ = criarNoNum($1); }
    | ID                			{ 
                                  $$ = criarNoId($1);
                                  // Verificar se a variável foi declarada
                                  if (!verificarDeclaracao($1)) {
                                      erros_semanticos++;
                                  }
                                }
    | TRUE              			{ $$ = criarNoOp('T', NULL, NULL); }
    | FALSE             			{ $$ = criarNoOp('F', NULL, NULL); }
    | FLOAT_NUM         			{ 
                                  float valor = atof($1);
                                  $$ = criarNoFloat(valor);
                                }
    | ID LPAREN RPAREN              { $$ = criarNoChamada($1, NULL); }
    | ID LPAREN element_list RPAREN { $$ = criarNoChamada($1, $3); }
    | list_expr         			{ $$ = $1; }
	;

list_expr:
    LBRACKET RBRACKET               { $$ = criarNoListaVazia(); }
    | LBRACKET element_list RBRACKET { $$ = $2; }
    ;

element_list:
    expr                            { $$ = criarNoLista($1, NULL); }
    | expr COMMA element_list       { $$ = criarNoLista($1, $3); }
    ;

%%

void yyerror(const char *s) {
    fprintf(stderr, "Erro de sintaxe: %s\n", s);
    fprintf(stderr, "Token inesperado: %s\n", yytext);
    fprintf(stderr, "Linha: %d\n", yylineno);
}
