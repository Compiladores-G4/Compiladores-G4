#ifndef AST_H
#define AST_H

#include "tipos.h"
#include <stdio.h>

// Funções básicas de criação de nós
NoAST *criarNoNum(int valor);
NoAST *criarNoId(char *nome);
NoAST *criarNoOp(char operador, NoAST *esq, NoAST *dir);

// Funções para nós de controle e estruturas
NoAST *criarNoAtribuicao(NoAST *id, NoAST *expressao);
NoAST *criarNoIf(NoAST *condicao, NoAST *entao, NoAST *senao);
NoAST *criarNoWhile(NoAST *condicao, NoAST *corpo);
NoAST *criarNoFor(NoAST *variavel, NoAST *inicio, NoAST *fim, NoAST *passo, NoAST *corpo);
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

void gerarCodigoC(NoAST *raiz, FILE *saida);

#endif
