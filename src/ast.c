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
  novo->esquerda = entao;
  novo->direita = senao; 
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
  novo->operador = tipo[0];      
  novo->direita = inicializacao;
  novo->esquerda = novo->condicao = novo->corpo = NULL;
  novo->proximoIrmao = NULL;
  return novo;
}

NoAST *criarNoFuncao(char *tipo, char *nome, NoAST *parametros, NoAST *corpo) {
  NoAST *novo = malloc(sizeof(NoAST));
  novo->tipo = NO_FUNCAO;
  strcpy(novo->nome, nome);
  novo->operador = tipo[0];
  novo->esquerda = parametros; 
  novo->corpo = corpo;  
  novo->direita = novo->condicao = NULL;
  novo->proximoIrmao = NULL;
  return novo;
}

NoAST *criarNoChamada(char *nome, NoAST *argumentos) {
  NoAST *novo = malloc(sizeof(NoAST));
  novo->tipo = NO_CHAMADA;
  strcpy(novo->nome, nome);
  novo->esquerda = argumentos;
  novo->direita = novo->condicao = novo->corpo = NULL;
  novo->proximoIrmao = NULL;
  return novo;
}

NoAST *criarNoBloco(NoAST *declaracoes) {
  NoAST *novo = malloc(sizeof(NoAST));
  novo->tipo = NO_BLOCO;
  novo->esquerda = declaracoes;
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

    if (raiz->proximoIrmao) {
      printf(", ");
      imprimirAST(raiz->proximoIrmao);
    }
  }
}

void imprimirASTDetalhada(NoAST *raiz, int nivel) {
  if (!raiz)
    return;

  for (int i = 0; i < nivel; i++) {
    printf("  ");
  }

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
    if (raiz->condicao) {
      for (int i = 0; i < nivel + 1; i++) {
        printf("  ");
      }
      printf("Condição:\n");
      imprimirASTDetalhada(raiz->condicao, nivel + 2);
    }
    if (raiz->esquerda) {
      for (int i = 0; i < nivel + 1; i++) {
        printf("  ");
      }
      printf("Bloco Then:\n");
      imprimirASTDetalhada(raiz->esquerda, nivel + 2);
    }
    if (raiz->direita) {
      for (int i = 0; i < nivel + 1; i++) {
        printf("  ");
      }
      printf("Bloco Else:\n");
      imprimirASTDetalhada(raiz->direita, nivel + 2);
    }
    break;
  case NO_WHILE:
    printf("While:\n");
    break;
  case NO_DECLARACAO:
    printf("Declaração: %c %s\n", raiz->operador, raiz->nome);
    break;
  case NO_FUNCAO:
    printf("Função: %c %s\n", raiz->operador, raiz->nome);
    if (raiz->esquerda) {
      for (int i = 0; i < nivel + 1; i++) {
        printf("  ");
      }
      printf("Parâmetros:\n");
      imprimirASTDetalhada(raiz->esquerda, nivel + 2);
    }
    if (raiz->corpo) {
      for (int i = 0; i < nivel + 1; i++) {
        printf("  ");
      }
      printf("Corpo:\n");
      imprimirASTDetalhada(raiz->corpo, nivel + 2);
    }
    break;
  case NO_CHAMADA:
    printf("Chamada: %s\n", raiz->nome);
    break;
  case NO_BLOCO:
    printf("Bloco:\n");
    break;
  }

  if (raiz->esquerda)
    imprimirASTDetalhada(raiz->esquerda, nivel + 1);
  if (raiz->direita)
    imprimirASTDetalhada(raiz->direita, nivel + 1);
  if (raiz->condicao)
    imprimirASTDetalhada(raiz->condicao, nivel + 1);
  if (raiz->corpo)
    imprimirASTDetalhada(raiz->corpo, nivel + 1);

  if (raiz->proximoIrmao)
    imprimirASTDetalhada(raiz->proximoIrmao, nivel);
}

void liberarAST(NoAST *raiz) {
  if (!raiz)
    return;

  liberarAST(raiz->esquerda);
  liberarAST(raiz->direita);
  liberarAST(raiz->condicao);
  liberarAST(raiz->corpo);
  liberarAST(raiz->proximoIrmao);

  free(raiz);
}
