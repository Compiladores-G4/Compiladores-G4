#ifndef TABELA_H
#define TABELA_H

#define TAM_NOME 64
#define TAM_TIPO 32

typedef struct Simbolo {
    char nome[TAM_NOME];
    char tipo[TAM_TIPO];
    struct Simbolo *prox;
} Simbolo;

void inserirSimbolo(char *nome, char *tipo);
Simbolo *buscarSimbolo(char *nome);
void imprimirTabela();

#endif // TABELA_H
