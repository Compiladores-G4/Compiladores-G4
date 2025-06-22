#ifndef TIPOS_H
#define TIPOS_H

typedef enum {
  NO_NUMERO,
  NO_FLOAT,        
  NO_ID,
  NO_OPERADOR,
  NO_ATRIBUICAO,
  NO_IF,
  NO_WHILE,
  NO_FOR,          
  NO_DECLARACAO,
  NO_FUNCAO,
  NO_CHAMADA,
  NO_BLOCO,
  NO_LISTA
} TipoNo;

typedef struct NoAST {
  TipoNo tipo;
  char operador;
  int valor;
  float valorFloat;      
  char nome[100];  struct NoAST *esquerda;
  struct NoAST *direita;
  struct NoAST *condicao;    
  struct NoAST *corpo;        
  struct NoAST *passo;     
  struct NoAST *proximoIrmao;
} NoAST;

typedef enum { TIPO_INT, TIPO_FLOAT, TIPO_CHAR, TIPO_VOID } TipoDado;

#endif
