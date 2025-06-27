#include "tabela.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Simbolo *tabela = NULL;
Escopo *escopoAtual = NULL;

void inicializarEscopos() {
  if (!escopoAtual) {
    escopoAtual = malloc(sizeof(Escopo));
    strcpy(escopoAtual->nome, "global");
    escopoAtual->pai = NULL;
    escopoAtual->prox = NULL;
  }
}

void criarEscopo(char *nome) {
  inicializarEscopos(); 
  
  Escopo *novo = malloc(sizeof(Escopo));
  strcpy(novo->nome, nome);
  novo->pai = escopoAtual;
  novo->prox = NULL;
  
  Escopo *ultimo = escopoAtual;
  while (ultimo->prox) ultimo = ultimo->prox;
  ultimo->prox = novo;
  
  escopoAtual = novo;
}

void sairEscopo() {
  if (escopoAtual && escopoAtual->pai) {
    escopoAtual = escopoAtual->pai;
  }
}

Escopo *obterEscopoAtual() {
  inicializarEscopos();
  return escopoAtual;
}

char *obterNomeEscopoAtual() {
  return obterEscopoAtual()->nome;
}

void inserirSimbolo(char *nome, char *tipo) {
  inserirSimboloComEscopo(nome, tipo, obterNomeEscopoAtual());
}

void inserirSimboloComEscopo(char *nome, char *tipo, char *escopo) {
  Simbolo *s = buscarSimboloNoEscopo(nome, escopo);
  if (s) {
    if (!strcmp(s->tipo, "desconhecido") && strcmp(tipo, "desconhecido")) {
      strcpy(s->tipo, tipo);
    }
    return;
  }
  
  Simbolo *novo = malloc(sizeof(Simbolo));
  if (!novo) {
    fprintf(stderr, "Erro: Falha na alocação de memória\n");
    return;
  }
  
  strcpy(novo->nome, nome);
  strcpy(novo->tipo, tipo);
  strcpy(novo->escopo, escopo);
  novo->valor = NULL;
  novo->prox = tabela;
  tabela = novo;
}

Simbolo *buscarSimboloNoEscopo(char *nome, char *escopo) {
  for (Simbolo *s = tabela; s; s = s->prox) {
    if (!strcmp(s->nome, nome) && !strcmp(s->escopo, escopo)) {
      return s;
    }
  }
  return NULL;
}

Simbolo *buscarSimbolo(char *nome) {
  for (Escopo *e = obterEscopoAtual(); e; e = e->pai) {
    Simbolo *s = buscarSimboloNoEscopo(nome, e->nome);
    if (s) return s;
  }
  return NULL;
}

void imprimirTabela() {
  printf("\n===== TABELA DE SÍMBOLOS =====\n");
  printf("Nome\tTipo\tEscopo\n");
  printf("---------------------------\n");
  for (Simbolo *s = tabela; s; s = s->prox) {
    printf("%s\t%s\t%s\n", s->nome, s->tipo, s->escopo);
  }
  printf("==============================\n");
}

void printEscopo(Escopo *e, int nivel) {
  if (!e) return;
  printf("%*s%s\n", nivel * 2, "", e->nome);
  for (Escopo *filho = e->prox; filho; filho = filho->prox) {
    printEscopo(filho, nivel + 1);
  }
}

void imprimirEscopos() {
  printf("\n===== ESTRUTURA DE ESCOPOS =====\n");
  printEscopo(escopoAtual ? escopoAtual : obterEscopoAtual(), 0);
  printf("=================================\n");
}

int verificarDeclaracao(char *nome) {
  if (!buscarSimbolo(nome)) {
    printf("Erro semântico: variável '%s' não declarada\n", nome);
    return 0;
  }
  return 1;
}

int tiposCompativeis(char *tipo1, char *tipo2) {
  if (!strcmp(tipo1, tipo2) || !strcmp(tipo1, "desconhecido") || !strcmp(tipo2, "desconhecido")) {
    return 1;
  }
  return (!strcmp(tipo1, "int") && !strcmp(tipo2, "float")) || 
         (!strcmp(tipo1, "float") && !strcmp(tipo2, "int"));
}

int verificarAtribuicao(char *destino, char *origem) {
  Simbolo *s_dest = buscarSimbolo(destino);
  Simbolo *s_orig = buscarSimbolo(origem);
  
  if (!s_dest || !s_orig || !tiposCompativeis(s_dest->tipo, s_orig->tipo)) {
    if (s_dest && s_orig) {
      printf("Erro semântico: atribuição incompatível de '%s' (%s) para '%s' (%s)\n", 
             origem, s_orig->tipo, destino, s_dest->tipo);
    }
    return 0;
  }
  return 1;
}

char *obterTipoResultante(char *tipo1, char *tipo2, char operador) {
  if (!strcmp(tipo1, "desconhecido") || !strcmp(tipo2, "desconhecido")) return "desconhecido";
  
  if (operador == '=' || operador == '!' || operador == '<' || operador == '>' || 
      operador == 'l' || operador == 'g') {
    return "bool";
  }
  
  if (!strcmp(tipo1, "int") && !strcmp(tipo2, "int")) return "int";
  if (!strcmp(tipo1, "float") || !strcmp(tipo2, "float"))

 return "float";
  
  if (!strcmp(tipo1, "bool") && !strcmp(tipo2, "bool")) {
    return (operador == '+' || operador == '-' || operador == '*' || operador == '/') ? "int" : "bool";
  }
  return "desconhecido";
}

int verificarOperacao(char *nome1, char *nome2, char operador) {
  Simbolo *s1 = buscarSimbolo(nome1);
  Simbolo *s2 = buscarSimbolo(nome2);
  
  if (!s1 || !s2) return 0;
  
  if ((operador == '&' || operador == '|') && 
      strcmp(s1->tipo, "bool") && strcmp(s2->tipo, "bool") && 
      strcmp(s1->tipo, "desconhecido") && strcmp(s2->tipo, "desconhecido")) {
    printf("Erro semântico: operador '%s' requer bool\n", operador == '&' ? "AND" : "OR");
    return 0;
  }
  
  if ((operador == '=' || operador == '!') && 
      strcmp(s1->tipo, "bool") && strcmp(s2->tipo, "bool") && 
      strcmp(s1->tipo, "desconhecido") && strcmp(s2->tipo, "desconhecido")) {
    printf("Erro semântico: operador '%c' requer bool\n", operador);
    return 0;
  }
  
  if ((operador == '+' || operador == '-' || operador == '*' || operador == '/' || operador == '%') && 
      strcmp(s1->tipo, "int") && strcmp(s1->tipo, "float") && strcmp(s1->tipo, "desconhecido") && 
      strcmp(s2->tipo, "int") && strcmp(s2->tipo, "float") && strcmp(s2->tipo, "desconhecido")) {
    printf("Erro semântico: operador '%c' requer numéricos\n", operador);
    return 0;
  }
  
  return 1;
}