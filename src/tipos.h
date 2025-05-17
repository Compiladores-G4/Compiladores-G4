#ifndef TIPOS_H
#define TIPOS_H

// Tipos de nós na AST
typedef enum {
    NO_NUMERO,
    NO_ID,
    NO_OPERADOR,
    NO_ATRIBUICAO,
    NO_IF,
    NO_WHILE,
    NO_DECLARACAO,
    NO_FUNCAO,
    NO_CHAMADA,
    NO_BLOCO
} TipoNo;

// Estrutura para nó da AST
typedef struct NoAST {
    TipoNo tipo;
    char operador;
    int valor;
    char nome[100];
    struct NoAST *esquerda;
    struct NoAST *direita;
    struct NoAST *condicao;  // Para nós if/while
    struct NoAST *corpo;     // Para nós if/while/função
    struct NoAST *proximoIrmao; // Para listas (parâmetros, argumentos)
} NoAST;

// Tipos de valores
typedef enum {
    TIPO_INT,
    TIPO_FLOAT,
    TIPO_CHAR,
    TIPO_VOID
} TipoDado;

#endif
