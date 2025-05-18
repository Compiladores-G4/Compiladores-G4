#ifndef AST_H
#define AST_H

#include <stdio.h>
#include "tipos.h"

// Funções básicas de criação de nós
NoAST *criarNoNum(int valor);
NoAST *criarNoId(char *nome);
NoAST *criarNoOp(char operador, NoAST *esq, NoAST *dir);

// Funções para nós de controle e estruturas
NoAST *criarNoAtribuicao(NoAST *id, NoAST *expressao);
NoAST *criarNoIf(NoAST *condicao, NoAST *entao, NoAST *senao);
NoAST *criarNoWhile(NoAST *condicao, NoAST *corpo);
NoAST *criarNoDeclaracao(char *tipo, char *nome, NoAST *inicializacao);
NoAST *criarNoFuncao(char *tipo, char *nome, NoAST *parametros, NoAST *corpo);
NoAST *criarNoChamada(char *nome, NoAST *argumentos);
NoAST *criarNoBloco(NoAST *declaracoes);

// Adiciona um nó irmão (para listas de parâmetros, argumentos, etc.)
NoAST *adicionarIrmao(NoAST *node, NoAST *irmao);

// Funções para visualização e gerenciamento da AST
void imprimirAST(NoAST *raiz);
void imprimirASTDetalhada(NoAST *raiz, int nivel);
void liberarAST(NoAST *raiz);

// Nova função: Gera código C a partir da AST
void gerarCodigoC(NoAST *raiz, FILE *saida);

#endif