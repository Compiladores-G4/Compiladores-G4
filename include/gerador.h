#ifndef GERADOR_H
#define GERADOR_H

#include "ast.h"
#include "tabela.h"

// Tipos de operação para código de três endereços
typedef enum {
    OP_ADD,     // +
    OP_SUB,     // -
    OP_MUL,     // *
    OP_DIV,     // /
    OP_MOD,     // %
    OP_ASSIGN,  // =
    OP_LABEL,   // Label para saltos
    OP_JUMP,    // Salto incondicional
    OP_CJUMP,   // Salto condicional
    OP_LT,      // <
    OP_GT,      // >
    OP_LE,      // <=
    OP_GE,      // >=
    OP_EQ,      // ==
    OP_NE,      // !=
    OP_AND,     // and
    OP_OR,      // or
    OP_NOT,     // not
    OP_PARAM,   // Parâmetro para função
    OP_CALL,    // Chamada de função
    OP_RETURN,  // Retorno de função
    OP_DEREF,   // Dereferenciar (a = *p)
    OP_ADDR,    // Endereço (a = &b)
    OP_LOAD,    // Carregar (temporário = variável)
    OP_STORE    // Armazenar (variável = temporário)
} TipoOperacao;

// Estrutura para uma instrução de código de três endereços
typedef struct Instrucao {
    TipoOperacao op;
    char resultado[64];  // Destino do resultado
    char arg1[64];       // Primeiro operando 
    char arg2[64];       // Segundo operando
    int rotulo;          // Usado para labels e saltos
    struct Instrucao *prox;
} Instrucao;

// Estrutura para manter o código gerado
typedef struct CodigoIntermediario {
    Instrucao *inicio;    // Primeira instrução
    Instrucao *fim;       // Última instrução
    int numTemporarios;   // Contador de variáveis temporárias
    int numRotulos;       // Contador de rótulos
} CodigoIntermediario;

// Funções para geração e manipulação de código
CodigoIntermediario* inicializarCodigoIntermediario();
void liberarCodigoIntermediario(CodigoIntermediario *codigo);
void adicionarInstrucao(CodigoIntermediario *codigo, TipoOperacao op, 
                       const char *resultado, const char *arg1, const char *arg2, int rotulo);
char* gerarTemporario(CodigoIntermediario *codigo);
int gerarRotulo(CodigoIntermediario *codigo);

// Funções principais para geração de código
CodigoIntermediario* gerarCodigoIntermediario(NoAST *raiz);
void gerarCodigoExpressao(CodigoIntermediario *codigo, NoAST *no, char *resultado);
void gerarCodigoDeclaracao(CodigoIntermediario *codigo, NoAST *no);
void gerarCodigoAtribuicao(CodigoIntermediario *codigo, NoAST *no);
void gerarCodigoCondicional(CodigoIntermediario *codigo, NoAST *no);
void gerarCodigoLaco(CodigoIntermediario *codigo, NoAST *no);
void gerarCodigoFor(CodigoIntermediario *codigo, NoAST *no);
void gerarCodigoFuncao(CodigoIntermediario *codigo, NoAST *no);
void gerarCodigoChamada(CodigoIntermediario *codigo, NoAST *no, char *resultado);

// Função para imprimir o código intermediário gerado
void imprimirCodigoIntermediario(CodigoIntermediario *codigo);

#endif // GERADOR_H
