#include "tabela.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Simbolo *tabela = NULL;
Escopo *escopoAtual = NULL;
Escopo *escoposRaiz = NULL;

// Inicializa a estrutura de escopos com o escopo global
void inicializarEscopos() {
  if (escoposRaiz == NULL) {
    escoposRaiz = malloc(sizeof(Escopo));
    strcpy(escoposRaiz->nome, "global");
    escoposRaiz->pai = NULL;
    escoposRaiz->prox = NULL;
    escopoAtual = escoposRaiz;
  }
}

// Cria um novo escopo como filho do escopo atual
void criarEscopo(char *nome) {
  if (escopoAtual == NULL) {
    inicializarEscopos();
  }
  
  Escopo *novo = malloc(sizeof(Escopo));
  strcpy(novo->nome, nome);
  novo->pai = escopoAtual;
  novo->prox = NULL;
  
  // Adicionar como próximo do último filho do escopo atual
  if (escopoAtual->prox == NULL) {
    escopoAtual->prox = novo;
  } else {
    Escopo *ultimo = escopoAtual->prox;
    while (ultimo->prox != NULL) {
      ultimo = ultimo->prox;
    }
    ultimo->prox = novo;
  }
  
  // Atualizar escopo atual
  escopoAtual = novo;
}

// Sai do escopo atual e volta para o escopo pai
void sairEscopo() {
  if (escopoAtual != NULL && escopoAtual->pai != NULL) {
    escopoAtual = escopoAtual->pai;
  }
}

// Retorna o escopo atual
Escopo *obterEscopoAtual() {
  if (escopoAtual == NULL) {
    inicializarEscopos();
  }
  return escopoAtual;
}

// Retorna o nome do escopo atual
char *obterNomeEscopoAtual() {
  if (escopoAtual == NULL) {
    inicializarEscopos();
  }
  return escopoAtual->nome;
}

// Insere um símbolo no escopo atual
void inserirSimbolo(char *nome, char *tipo) {
  char *escopoNome = obterNomeEscopoAtual();
  inserirSimboloComEscopo(nome, tipo, escopoNome);
}

// Insere um símbolo em um escopo específico
void inserirSimboloComEscopo(char *nome, char *tipo, char *escopo) {
  // Verifica se o símbolo já existe no mesmo escopo
  Simbolo *s = buscarSimboloNoEscopo(nome, escopo);
  if (s != NULL) {
    // O símbolo já existe, apenas atualiza o tipo se necessário
    if (strcmp(s->tipo, "desconhecido") == 0 && strcmp(tipo, "desconhecido") != 0) {
      strcpy(s->tipo, tipo);
    }
    return;
  }
  
  Simbolo *novo = malloc(sizeof(Simbolo));
  strcpy(novo->nome, nome);
  strcpy(novo->tipo, tipo);
  strcpy(novo->escopo, escopo);
  novo->valor = NULL;
  novo->prox = NULL;
  
  if (tabela == NULL) {
    tabela = novo;
  } else {
    Simbolo *last = tabela;
    while (last->prox)
      last = last->prox;
    last->prox = novo;
  }
}

// Busca um símbolo na cadeia de escopos (começa no atual e sobe até o global)
Simbolo *buscarSimbolo(char *nome) {
  return buscarSimboloCadeiaEscopos(nome);
}

// Busca um símbolo em um escopo específico
Simbolo *buscarSimboloNoEscopo(char *nome, char *escopo) {
  for (Simbolo *s = tabela; s; s = s->prox)
    if (strcmp(s->nome, nome) == 0 && strcmp(s->escopo, escopo) == 0)
      return s;
  return NULL;
}

// Busca um símbolo na cadeia de escopos
Simbolo *buscarSimboloCadeiaEscopos(char *nome) {
  if (escopoAtual == NULL) {
    inicializarEscopos();
  }
  
  // Começa a busca no escopo atual
  Escopo *escopo = escopoAtual;
  
  // Percorre a cadeia de escopos até encontrar o símbolo ou chegar à raiz
  while (escopo != NULL) {
    Simbolo *s = buscarSimboloNoEscopo(nome, escopo->nome);
    if (s != NULL) {
      return s;
    }
    escopo = escopo->pai;
  }
  
  return NULL;
}

// Atualiza o valor de um símbolo
void atualizarSimboloValor(char *nome, void *valor) {
  Simbolo *s = buscarSimbolo(nome);
  if (s) {
    if (s->valor != NULL) {
      // Libera o valor antigo para evitar vazamento de memória
      free(s->valor);
    }
    s->valor = valor;
  } else {
    printf("Erro: símbolo %s não encontrado na tabela.\n", nome);
  }
}

// Imprime a tabela de símbolos com informações de escopo
void imprimirTabela() {
  printf("\nTabela de Símbolos:\n");
  printf("----------------------------------------\n");
  printf("| %-15s | %-10s | %-15s |\n", "Nome", "Tipo", "Escopo");
  printf("----------------------------------------\n");
  
  for (Simbolo *s = tabela; s; s = s->prox)
    printf("| %-15s | %-10s | %-15s |\n", s->nome, s->tipo, s->escopo);
  
  printf("----------------------------------------\n");
}

// Imprime a árvore de escopos
void imprimirEscopos() {
  printf("\nHierarquia de Escopos:\n");
  
  if (escoposRaiz == NULL) {
    printf("Nenhum escopo definido.\n");
    return;
  }
  
  // Função auxiliar para imprimir escopos com identação
  void imprimirEscopoRecursivo(Escopo *escopo, int nivel) {
    if (escopo == NULL) return;
    
    // Imprime o escopo atual com identação
    for (int i = 0; i < nivel; i++) printf("  ");
    printf("- %s\n", escopo->nome);
    
    // Imprime os filhos deste escopo
    Escopo *filho = escopo->prox;
    while (filho != NULL && filho->pai == escopo) {
      imprimirEscopoRecursivo(filho, nivel + 1);
      filho = filho->prox;
    }
  }
  
  imprimirEscopoRecursivo(escoposRaiz, 0);
}

// Libera a memória alocada para todos os escopos
void destruirEscopos() {
  Escopo *atual = escoposRaiz;
  Escopo *proximo;
  
  while (atual != NULL) {
    proximo = atual->prox;
    free(atual);
    atual = proximo;
  }
  
  escoposRaiz = NULL;
  escopoAtual = NULL;
}

// ANÁLISE SEMÂNTICA

// Verifica se um símbolo foi declarado antes de ser usado
int verificarDeclaracao(char *nome) {
  Simbolo *s = buscarSimbolo(nome);
  if (s == NULL) {
    printf("Erro semântico: variável '%s' usada sem declaração prévia\n", nome);
    return 0;
  }
  return 1;
}

// Verifica compatibilidade entre tipos
int tiposCompativeis(char *tipo1, char *tipo2) {
  // Se algum dos tipos for desconhecido, permitimos a operação
  if (strcmp(tipo1, "desconhecido") == 0 || strcmp(tipo2, "desconhecido") == 0) {
    return 1;
  }
  
  // Tipos idênticos são compatíveis
  if (strcmp(tipo1, tipo2) == 0) {
    return 1;
  }
  
  // int e float são compatíveis entre si
  if ((strcmp(tipo1, "int") == 0 && strcmp(tipo2, "float") == 0) ||
      (strcmp(tipo1, "float") == 0 && strcmp(tipo2, "int") == 0)) {
    return 1;
  }
  
  // bool é compatível com int (0 = falso, não-zero = verdadeiro)
  if ((strcmp(tipo1, "bool") == 0 && strcmp(tipo2, "int") == 0) ||
      (strcmp(tipo1, "int") == 0 && strcmp(tipo2, "bool") == 0)) {
    return 1;
  }
  
  // Outros casos não são compatíveis
  return 0;
}

// Verifica se uma operação entre dois símbolos é permitida
int verificarOperacao(char *nome1, char *nome2, char operador) {
  Simbolo *s1 = buscarSimbolo(nome1);
  Simbolo *s2 = buscarSimbolo(nome2);
  
  if (!s1 || !s2) {
    return 0; // Um dos símbolos não foi encontrado
  }
  
  // Para operações lógicas (==, !=, <, >, <=, >=)
  if (operador == '=' || operador == '!' || operador == '<' || operador == '>' || operador == 'l' || operador == 'g') {
    return tiposCompativeis(s1->tipo, s2->tipo);
  }
  
  // Para operações aritméticas (+, -, *, /)
  if (operador == '+' || operador == '-' || operador == '*' || operador == '/') {
    // Verificar se ambos são numéricos (int ou float)
    if ((strcmp(s1->tipo, "int") == 0 || strcmp(s1->tipo, "float") == 0) &&
        (strcmp(s2->tipo, "int") == 0 || strcmp(s2->tipo, "float") == 0)) {
      return 1;
    }
    return 0;
  }
  
  return 0; // Operador desconhecido
}

// Determina o tipo resultante de uma operação
char* obterTipoResultante(char *tipo1, char *tipo2, char operador) {
  // Para operações lógicas, o resultado é bool
  if (operador == '=' || operador == '!' || operador == '<' || operador == '>' || operador == 'l' || operador == 'g') {
    return "bool";
  }
  
  // Para operações aritméticas
  if (operador == '+' || operador == '-' || operador == '*' || operador == '/') {
    // Se algum dos operandos for float, o resultado é float
    if (strcmp(tipo1, "float") == 0 || strcmp(tipo2, "float") == 0) {
      return "float";
    }
    // Caso contrário, o resultado é int
    return "int";
  }
  
  return "desconhecido"; // Operador desconhecido
}

// Verifica se uma atribuição é válida
int verificarAtribuicao(char *destino, char *origem) {
  Simbolo *dest = buscarSimbolo(destino);
  Simbolo *orig = buscarSimbolo(origem);
  
  if (!dest || !orig) {
    return 0; // Um dos símbolos não foi encontrado
  }
  
  return tiposCompativeis(dest->tipo, orig->tipo);
}
