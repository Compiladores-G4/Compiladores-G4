%{ 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tipos.h"
#include "ast.h"
#include "tabela.h"

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
#include "ast.h"
}

%union {
	char* string;
	int intValue; // Adicionado para expressões
	NoAST* ast;    // Ponteiro para nós da AST
}

%token DEF RETURN IF ELSE ELIF WHILE FOR IN RANGE
%token <string> TRUE FALSE
%token <intValue> NUM
%token <string> ID
%token EQ NE LT GT LE GE ASSIGN
%token AND OR NOT
%token PLUS MINUS TIMES DIVIDE MOD
%token LPAREN RPAREN LBRACE RBRACE COMMA COLON SEMICOLON
%token INDENT DEDENT
%token ARROW
%token TYPE_INT TYPE_FLOAT TYPE_BOOL

%type <ast> expr variable_declaration value statement statements program conditional_stmt else_part

%%

program:
	| statements {
		raiz = $1;
		$$ = raiz;
	}
	| function_stmts
	;

function_stmts:
	function_stmts function_stmt |
	function_stmt
	;

function_stmt:
	DEF ID LPAREN RPAREN COLON INDENT statements DEDENT { }
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
		/* Representa elif como um if dentro do bloco else */
		$$ = criarNoIf($2, $5, $7);
	}
	;

variable_declaration:
	ID ASSIGN value { 
		NoAST *id_node = criarNoId($1);
		$$ = criarNoAtribuicao(id_node, $3);
	}
	;

value:
	TRUE     		{ $$ = criarNoNum(1); }
	| FALSE    	{ $$ = criarNoNum(0); }
	| expr			{ $$ = $1; }
	;

expr:
	expr PLUS expr    			{ $$ = criarNoOp('+', $1, $3); }
| expr MINUS expr   			{ $$ = criarNoOp('-', $1, $3); }
| expr TIMES expr   			{ $$ = criarNoOp('*', $1, $3); }
| expr DIVIDE expr  			{ $$ = criarNoOp('/', $1, $3); }
| expr EQ expr     			{ $$ = criarNoOp('=', $1, $3); }
| expr NE expr     			{ $$ = criarNoOp('!', $1, $3); }
| expr LT expr     			{ $$ = criarNoOp('<', $1, $3); }
| expr GT expr     			{ $$ = criarNoOp('>', $1, $3); }
| expr LE expr     			{ $$ = criarNoOp('l', $1, $3); }
| expr GE expr     			{ $$ = criarNoOp('g', $1, $3); }
| LPAREN expr RPAREN			{ $$ = $2; }
| NUM               			{ $$ = criarNoNum($1); }
| ID                			{ 
														$$ = criarNoId($1);
														inserirSimbolo($1, "int");
													}
| TRUE              			{ $$ = criarNoNum(1); }
| FALSE             			{ $$ = criarNoNum(0); }
	;
    
%%

void yyerror(const char *s) {
    fprintf(stderr, "Erro de sintaxe: %s\n", s);
}
