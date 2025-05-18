#include "ast.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

NoAST *criarNoNum(int valor) {
  NoAST *novo = malloc(sizeof(NoAST));
  novo->tipo = NO_NUMERO;
  novo->operador = ' ';
  novo->valor = valor;
  novo->esquerda = novo->direita = NULL;
  novo->condicao = novo->corpo = NULL;
  novo->proximoIrmao = NULL;
  return novo;
}

NoAST *criarNoId(char *nome) {
  NoAST *novo = malloc(sizeof(NoAST));
  novo->tipo = NO_ID;
  novo->operador = 'i';
  strcpy(novo->nome, nome);
  novo->esquerda = novo->direita = NULL;
  novo->condicao = novo->corpo = NULL;
  novo->proximoIrmao = NULL;
  return novo;
}

NoAST *criarNoOp(char operador, NoAST *esq, NoAST *dir) {
  NoAST *novo = malloc(sizeof(NoAST));
  novo->tipo = NO_OPERADOR;
  novo->operador = operador;
  novo->esquerda = esq;
  novo->direita = dir;
  novo->condicao = novo->corpo = NULL;
  novo->proximoIrmao = NULL;
  return novo;
}

NoAST *criarNoAtribuicao(NoAST *id, NoAST *expressao) {
  NoAST *novo = malloc(sizeof(NoAST));
  novo->tipo = NO_ATRIBUICAO;
  novo->operador = '=';
  novo->esquerda = id;
  novo->direita = expressao;
  novo->condicao = novo->corpo = NULL;
  novo->proximoIrmao = NULL;
  return novo;
}

NoAST *criarNoIf(NoAST *condicao, NoAST *entao, NoAST *senao) {
  NoAST *novo = malloc(sizeof(NoAST));
  novo->tipo = NO_IF;
  novo->condicao = condicao;
  novo->esquerda = entao; // Bloco "então"
  novo->direita = senao;  // Bloco "senão" (pode ser NULL)
  novo->corpo = NULL;
  novo->proximoIrmao = NULL;
  return novo;
}

NoAST *criarNoWhile(NoAST *condicao, NoAST *corpo) {
  NoAST *novo = malloc(sizeof(NoAST));
  novo->tipo = NO_WHILE;
  novo->condicao = condicao;
  novo->corpo = corpo;
  novo->esquerda = novo->direita = NULL;
  novo->proximoIrmao = NULL;
  return novo;
}

NoAST *criarNoDeclaracao(char *tipo, char *nome, NoAST *inicializacao) {
  NoAST *novo = malloc(sizeof(NoAST));
  novo->tipo = NO_DECLARACAO;
  strcpy(novo->nome, nome);
  // Tipo é armazenado no operador para simplicidade
  // Poderia ser melhorado com um campo específico para tipo
  novo->operador = tipo[0];      // 'i' para int, 'f' para float, etc.
  novo->direita = inicializacao; // Expressão de inicialização (pode ser NULL)
  novo->esquerda = novo->condicao = novo->corpo = NULL;
  novo->proximoIrmao = NULL;
  return novo;
}

NoAST *criarNoFuncao(char *tipo, char *nome, NoAST *parametros, NoAST *corpo) {
  NoAST *novo = malloc(sizeof(NoAST));
  novo->tipo = NO_FUNCAO;
  strcpy(novo->nome, nome);
  novo->operador = tipo[0];    // Tipo de retorno
  novo->esquerda = parametros; // Lista de parâmetros
  novo->corpo = corpo;         // Bloco de código
  novo->direita = novo->condicao = NULL;
  novo->proximoIrmao = NULL;
  return novo;
}

NoAST *criarNoChamada(char *nome, NoAST *argumentos) {
  NoAST *novo = malloc(sizeof(NoAST));
  novo->tipo = NO_CHAMADA;
  strcpy(novo->nome, nome);
  novo->esquerda = argumentos; // Lista de argumentos
  novo->direita = novo->condicao = novo->corpo = NULL;
  novo->proximoIrmao = NULL;
  return novo;
}

NoAST *criarNoBloco(NoAST *declaracoes) {
  NoAST *novo = malloc(sizeof(NoAST));
  novo->tipo = NO_BLOCO;
  novo->esquerda = declaracoes; // Primeiro elemento da lista
  novo->direita = novo->condicao = novo->corpo = NULL;
  novo->proximoIrmao = NULL;
  return novo;
}

NoAST *adicionarIrmao(NoAST *node, NoAST *irmao) {
  if (!node)
    return irmao;

  NoAST *temp = node;
  while (temp->proximoIrmao != NULL) {
    temp = temp->proximoIrmao;
  }
  temp->proximoIrmao = irmao;
  return node;
}

void imprimirAST(NoAST *raiz) {
  if (raiz) {
    switch (raiz->tipo) {
    case NO_NUMERO:
      printf("%d", raiz->valor);
      break;
    case NO_ID:
      printf("%s", raiz->nome);
      break;
    case NO_OPERADOR:
      printf("(");
      imprimirAST(raiz->esquerda);
      printf(" %c ", raiz->operador);
      imprimirAST(raiz->direita);
      printf(")");
      break;
    case NO_ATRIBUICAO:
      imprimirAST(raiz->esquerda);
      printf(" = ");
      imprimirAST(raiz->direita);
      break;
    case NO_IF:
      printf("if (");
      imprimirAST(raiz->condicao);
      printf(") ");
      imprimirAST(raiz->esquerda);
      if (raiz->direita) {
        printf(" else ");
        imprimirAST(raiz->direita);
      }
      break;
    case NO_WHILE:
      printf("while (");
      imprimirAST(raiz->condicao);
      printf(") ");
      imprimirAST(raiz->corpo);
      break;
    case NO_DECLARACAO:
      printf("%c %s", raiz->operador, raiz->nome);
      if (raiz->direita) {
        printf(" = ");
        imprimirAST(raiz->direita);
      }
      break;
    case NO_FUNCAO:
      printf("%c %s(", raiz->operador, raiz->nome);
      imprimirAST(raiz->esquerda);
      printf(") ");
      imprimirAST(raiz->corpo);
      break;
    case NO_CHAMADA:
      printf("%s(", raiz->nome);
      imprimirAST(raiz->esquerda);
      printf(")");
      break;
    case NO_BLOCO:
      printf("{ ");
      imprimirAST(raiz->esquerda);
      printf(" }");
      break;
    }

    // Imprimir irmãos (para listas)
    if (raiz->proximoIrmao) {
      printf(", ");
      imprimirAST(raiz->proximoIrmao);
    }
  }
}

void imprimirASTDetalhada(NoAST *raiz, int nivel) {
  if (!raiz)
    return;

  // Indentação
  for (int i = 0; i < nivel; i++) {
    printf("  ");
  }

  // Imprime informações do nó atual
  switch (raiz->tipo) {
  case NO_NUMERO:
    printf("Número: %d\n", raiz->valor);
    break;
  case NO_ID:
    printf("Identificador: %s\n", raiz->nome);
    break;
  case NO_OPERADOR:
    printf("Operador: %c\n", raiz->operador);
    break;
  case NO_ATRIBUICAO:
    printf("Atribuição:\n");
    break;
  case NO_IF:
    printf("If:\n");
    break;
  case NO_WHILE:
    printf("While:\n");
    break;
  case NO_DECLARACAO:
    printf("Declaração: %c %s\n", raiz->operador, raiz->nome);
    break;
  case NO_FUNCAO:
    printf("Função: %c %s\n", raiz->operador, raiz->nome);
    break;
  case NO_CHAMADA:
    printf("Chamada: %s\n", raiz->nome);
    break;
  case NO_BLOCO:
    printf("Bloco:\n");
    break;
  }

  // Visita filhos recursivamente
  if (raiz->esquerda)
    imprimirASTDetalhada(raiz->esquerda, nivel + 1);
  if (raiz->direita)
    imprimirASTDetalhada(raiz->direita, nivel + 1);
  if (raiz->condicao)
    imprimirASTDetalhada(raiz->condicao, nivel + 1);
  if (raiz->corpo)
    imprimirASTDetalhada(raiz->corpo, nivel + 1);

  // Visita irmãos no mesmo nível
  if (raiz->proximoIrmao)
    imprimirASTDetalhada(raiz->proximoIrmao, nivel);
}

void liberarAST(NoAST *raiz) {
  if (!raiz)
    return;

  // Libera todos os nós recursivamente
  liberarAST(raiz->esquerda);
  liberarAST(raiz->direita);
  liberarAST(raiz->condicao);
  liberarAST(raiz->corpo);
  liberarAST(raiz->proximoIrmao);

  free(raiz);
}
