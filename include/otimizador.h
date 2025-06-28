#ifndef OTIMIZADOR_H
#define OTIMIZADOR_H

#include "gerador.h"

typedef struct {
    char nome[64];
    int usada; 
} VariavelUso;

typedef struct {
    VariavelUso *variaveis;
    int tamanho;
    int capacidade;
} TabelaUsoVariaveis;

CodigoIntermediario* otimizarCodigoIntermediario(CodigoIntermediario *codigo);

void removerCodigoMorto(CodigoIntermediario *codigo);
void simplificarExpressoes(CodigoIntermediario *codigo);
void propagarConstantes(CodigoIntermediario *codigo);
void eliminarSubexpressoesComuns(CodigoIntermediario *codigo);

TabelaUsoVariaveis* criarTabelaUsoVariaveis();
void liberarTabelaUsoVariaveis(TabelaUsoVariaveis *tabela);
void marcarVariavelUsada(TabelaUsoVariaveis *tabela, const char *nome);
int verificarVariavelUsada(TabelaUsoVariaveis *tabela, const char *nome);
void analisarUsoVariaveis(CodigoIntermediario *codigo, TabelaUsoVariaveis *tabela);

#endif // OTIMIZADOR_H
