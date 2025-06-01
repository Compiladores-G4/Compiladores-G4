#ifndef GERADOR_H
#define GERADOR_H

#include "ast.h"
#include "tabela.h"

// Tipos de operações para o código intermediário
typedef enum {
    OP_ADD,         // Adição
    OP_SUB,         // Subtração
    OP_MUL,         // Multiplicação
    OP_DIV,         // Divisão
    OP_MOD,         // Módulo (resto da divisão)
    OP_ASSIGN,      // Atribuição
    OP_LABEL,       // Rótulo
    OP_LABEL_FUNC,  // Rótulo de função
    OP_JUMP,        // Salto incondicional
    OP_CJUMP,       // Salto condicional
    OP_LT,          // Menor que
    OP_GT,          // Maior que
    OP_LE,          // Menor ou igual
    OP_GE,          // Maior ou igual
    OP_EQ,          // Igual
    OP_NE,          // Diferente
    OP_AND,         // Operação lógica AND
    OP_OR,          // Operação lógica OR
    OP_NOT,         // Operação lógica NOT
    OP_PARAM,       // Parâmetro de função
    OP_ARG,         // Argumento para chamada de função
    OP_CALL,        // Chamada de função
    OP_RETURN,      // Retorno de função
    OP_LOAD,        // Carrega valor de variável
    OP_STORE,       // Armazena valor em variável
    OP_CHECK_TYPE,  // Verifica tipo de variável/parâmetro
    OP_CHECK_ARGS   // Verifica número de argumentos
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

// Funções para geração de código intermediário
CodigoIntermediario* inicializarCodigoIntermediario();
void liberarCodigoIntermediario(CodigoIntermediario *codigo);
void adicionarInstrucao(CodigoIntermediario *codigo, TipoOperacao op, 
                        const char *resultado, const char *arg1, const char *arg2, int rotulo);
char* gerarTemporario(CodigoIntermediario *codigo);
int gerarRotulo(CodigoIntermediario *codigo);
TipoOperacao mapearOperador(char op);
void gerarCodigoExpressao(CodigoIntermediario *codigo, NoAST *no, char *resultado);
void gerarCodigoDeclaracao(CodigoIntermediario *codigo, NoAST *no);
void gerarCodigoAtribuicao(CodigoIntermediario *codigo, NoAST *no);
void gerarCodigoBloco(CodigoIntermediario *codigo, NoAST *bloco);
void gerarCodigoCondicional(CodigoIntermediario *codigo, NoAST *no);
void gerarCodigoLaco(CodigoIntermediario *codigo, NoAST *no);
void gerarCodigoFor(CodigoIntermediario *codigo, NoAST *no);
void gerarCodigoFuncao(CodigoIntermediario *codigo, NoAST *no);
void gerarCodigoChamada(CodigoIntermediario *codigo, NoAST *no, char *resultado);
void gerarCodigo(CodigoIntermediario *codigo, NoAST *raiz);
CodigoIntermediario* gerarCodigoIntermediario(NoAST *raiz);
void imprimirCodigoIntermediario(CodigoIntermediario *codigo);

#endif // GERADOR_H
