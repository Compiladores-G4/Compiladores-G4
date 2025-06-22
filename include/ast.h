#ifndef AST_H
#define AST_H

#include "tipos.h"
#include <stdio.h>

NoAST *criarNoNum(int valor);
NoAST *criarNoFloat(float valor);  
NoAST *criarNoId(char *nome);
NoAST *criarNoOp(char operador, NoAST *esq, NoAST *dir);

NoAST *criarNoAtribuicao(NoAST *id, NoAST *expressao);
NoAST *criarNoIf(NoAST *condicao, NoAST *entao, NoAST *senao);
NoAST *criarNoWhile(NoAST *condicao, NoAST *corpo);
NoAST *criarNoFor(NoAST *variavel, NoAST *inicio, NoAST *fim, NoAST *passo, NoAST *corpo);
NoAST *criarNoDeclaracao(char *tipo, char *nome, NoAST *inicializacao);
NoAST *criarNoFuncao(char *tipo, char *nome, NoAST *parametros, NoAST *corpo);
NoAST *criarNoChamada(char *nome, NoAST *argumentos);
NoAST *criarNoBloco(NoAST *declaracoes);
NoAST *criarNoListaVazia(void);
NoAST *criarNoLista(NoAST *primeiro, NoAST *resto);

NoAST *adicionarIrmao(NoAST *node, NoAST *irmao);

void imprimirAST(NoAST *raiz);
void imprimirASTDetalhada(NoAST *raiz, int nivel);
void liberarAST(NoAST *raiz);

void gerarCodigoC(NoAST *raiz, FILE *saida);

extern int nivelIndentacao;

char* inferirTipoExpressao(NoAST *expr);
int variavelJaDeclarada(char *nome);
void marcarVariavelDeclarada(char *nome);
void liberarVariaveisDeclaradas();

#endif
