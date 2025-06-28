#ifndef GERADOR_H
#define GERADOR_H

#include "ast.h"
#include "tabela.h"

typedef enum {
    OP_ADD,     
    OP_SUB,     
    OP_MUL,     
    OP_DIV,     
    OP_MOD,     
    OP_ASSIGN,  
    OP_LABEL,   
    OP_JUMP,   
    OP_CJUMP,   
    OP_LT,      
    OP_GT,      
    OP_LE,      
    OP_GE,      
    OP_EQ,      
    OP_NE,      
    OP_AND,     
    OP_OR,      
    OP_NOT,     
    OP_PARAM,   
    OP_CALL,    
    OP_RETURN, 
    OP_DEREF,   
    OP_ADDR,    
    OP_LOAD,    
    OP_STORE    
} TipoOperacao;

typedef struct Instrucao {
    TipoOperacao op;
    char resultado[64];  
    char arg1[64];      
    char arg2[64];       
    int rotulo;          
    struct Instrucao *prox;
} Instrucao;

typedef struct CodigoIntermediario {
    Instrucao *inicio;    
    Instrucao *fim;       
    int numTemporarios;   
    int numRotulos;      
} CodigoIntermediario;

CodigoIntermediario* inicializarCodigoIntermediario();
void liberarCodigoIntermediario(CodigoIntermediario *codigo);
void adicionarInstrucao(CodigoIntermediario *codigo, TipoOperacao op, 
                       const char *resultado, const char *arg1, const char *arg2, int rotulo);
char* gerarTemporario(CodigoIntermediario *codigo);
int gerarRotulo(CodigoIntermediario *codigo);

CodigoIntermediario* gerarCodigoIntermediario(NoAST *raiz);
void gerarCodigoExpressao(CodigoIntermediario *codigo, NoAST *no, char *resultado);
void gerarCodigoDeclaracao(CodigoIntermediario *codigo, NoAST *no);
void gerarCodigoAtribuicao(CodigoIntermediario *codigo, NoAST *no);
void gerarCodigoCondicional(CodigoIntermediario *codigo, NoAST *no);
void gerarCodigoLaco(CodigoIntermediario *codigo, NoAST *no);
void gerarCodigoFor(CodigoIntermediario *codigo, NoAST *no);
void gerarCodigoFuncao(CodigoIntermediario *codigo, NoAST *no);
void gerarCodigoChamada(CodigoIntermediario *codigo, NoAST *no, char *resultado);

void imprimirCodigoIntermediario(CodigoIntermediario *codigo);

#endif // GERADOR_H
