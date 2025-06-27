#ifndef OTIMIZADOR_H
#define OTIMIZADOR_H

#include "gerador.h"

// Estrutura para rastrear uso de variáveis
typedef struct {
    char nome[64];
    int usada;  // 1 se a variável é usada, 0 caso contrário
} VariavelUso;

// Estrutura para armazenar informações sobre variáveis
typedef struct {
    VariavelUso *variaveis;
    int tamanho;
    int capacidade;
} TabelaUsoVariaveis;

// Funções principais de otimização
CodigoIntermediario* otimizarCodigoIntermediario(CodigoIntermediario *codigo);

// Otimizações específicas
void removerCodigoMorto(CodigoIntermediario *codigo);
void simplificarExpressoes(CodigoIntermediario *codigo);
void propagarConstantes(CodigoIntermediario *codigo);
void eliminarSubexpressoesComuns(CodigoIntermediario *codigo);

// Funções auxiliares
TabelaUsoVariaveis* criarTabelaUsoVariaveis();
void liberarTabelaUsoVariaveis(TabelaUsoVariaveis *tabela);
void marcarVariavelUsada(TabelaUsoVariaveis *tabela, const char *nome);
int verificarVariavelUsada(TabelaUsoVariaveis *tabela, const char *nome);
void analisarUsoVariaveis(CodigoIntermediario *codigo, TabelaUsoVariaveis *tabela);

#endif // OTIMIZADOR_H
