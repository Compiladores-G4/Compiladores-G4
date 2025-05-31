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
%token LPAREN RPAREN LBRACE RBRACE COMMA COLON SEMICOLON
%token INDENT DEDENT
%token ARROW
%token TYPE_INT TYPE_FLOAT TYPE_BOOL

%type <ast> expr variable_declaration value statement statements program conditional_stmt else_part function_stmt function_stmts
%type <tipo> expr_tipo

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
        criarEscopo($2); // Cria um escopo para a função
    } INDENT statements DEDENT {
		$$ = criarNoFuncao("void", $2, NULL, $8);
        sairEscopo(); // Sai do escopo da função quando terminar
	}
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
	| conditional_stmt { $$ = $1; }
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
	ID ASSIGN value { 
		NoAST *id_node = criarNoId($1);
		$$ = criarNoAtribuicao(id_node, $3);
		
		// Inferir o tipo com base no valor e inserir na tabela de símbolos
		char *tipo;
		if ($3->tipo == NO_NUMERO) {
			tipo = "int";
		} else if ($3->tipo == NO_OPERADOR && $3->operador == 'f') {
			tipo = "float";
		} else if ($3->tipo == NO_OPERADOR && ($3->operador == 'T' || $3->operador == 'F')) {
			tipo = "bool";
		} else {
			tipo = "desconhecido";
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

value:
	TRUE     		{ $$ = criarNoOp('T', NULL, NULL); }
	| FALSE    	    { $$ = criarNoOp('F', NULL, NULL); }
	| expr			{ $$ = $1; }
	| FLOAT_NUM     { $$ = criarNoOp('f', NULL, NULL); /* Representando float */ }
	;

expr:
	expr PLUS expr    			{ 
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
                                  // Verificar se o identificador foi declarado antes de uso
                                  if (!verificarDeclaracao($1)) {
                                      erros_semanticos++;
                                      // Inserir com tipo desconhecido para evitar erros em cascata
                                      inserirSimbolo($1, "desconhecido");
                                  }
                                }
    | TRUE              			{ $$ = criarNoOp('T', NULL, NULL); }
    | FALSE             			{ $$ = criarNoOp('F', NULL, NULL); }
    | FLOAT_NUM         			{ $$ = criarNoOp('f', NULL, NULL); /* Representando float */ }
	;

expr_tipo:
    expr                        {
                                  if ($1->tipo == NO_NUMERO) {
                                      $$ = "int";
                                  } else if ($1->tipo == NO_ID) {
                                      Simbolo *s = buscarSimbolo($1->nome);
                                      if (s) $$ = s->tipo;
                                      else $$ = "desconhecido";
                                  } else if ($1->tipo == NO_OPERADOR) {
                                      switch($1->operador) {
                                          case 'T':
                                          case 'F':
                                              $$ = "bool";
                                              break;
                                          case 'f':
                                              $$ = "float";
                                              break;
                                          case '+':
                                          case '-':
                                          case '*':
                                          case '/':
                                              // Aqui precisaria verificar os tipos dos operandos
                                              if ($1->esquerda && $1->direita) {
                                                  char *tipo_esq = NULL;
                                                  char *tipo_dir = NULL;
                                                  
                                                  if ($1->esquerda->tipo == NO_ID) {
                                                      Simbolo *s = buscarSimbolo($1->esquerda->nome);
                                                      if (s) tipo_esq = s->tipo;
                                                  }
                                                  
                                                  if ($1->direita->tipo == NO_ID) {
                                                      Simbolo *s = buscarSimbolo($1->direita->nome);
                                                      if (s) tipo_dir = s->tipo;
                                                  }
                                                  
                                                  if (tipo_esq && tipo_dir) {
                                                      $$ = obterTipoResultante(tipo_esq, tipo_dir, $1->operador);
                                                  } else {
                                                      $$ = "desconhecido";
                                                  }
                                              } else {
                                                  $$ = "desconhecido";
                                              }
                                              break;
                                          default:
                                              $$ = "desconhecido";
                                      }
                                  } else {
                                      $$ = "desconhecido";
                                  }
                                }
    ;
    
%%

void yyerror(const char *s) {
    fprintf(stderr, "Erro de sintaxe: %s\n", s);
}
