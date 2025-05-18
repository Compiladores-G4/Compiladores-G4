#include "ast.h"
#include "tabela.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

NoAST *criarNoNum(int valor) {
    NoAST *novo = malloc(sizeof(NoAST));
    novo->tipo = NO_NUMERO;
    novo->operador = ' ';
    novo->valor = valor;
    novo->esquerda = novo->direita = NULL;
    novo->condicao = novo->corpo = NULL;
    novo->proximoIrmao = NULL;
    return novo;
}

NoAST *criarNoId(char *nome) {
    NoAST *novo = malloc(sizeof(NoAST));
    novo->tipo = NO_ID;
    novo->operador = 'i';
    strcpy(novo->nome, nome);
    novo->esquerda = novo->direita = NULL;
    novo->condicao = novo->corpo = NULL;
    novo->proximoIrmao = NULL;
    return novo;
}

NoAST *criarNoOp(char operador, NoAST *esq, NoAST *dir) {
    NoAST *novo = malloc(sizeof(NoAST));
    novo->tipo = NO_OPERADOR;
    novo->operador = operador;
    novo->esquerda = esq;
    novo->direita = dir;
    novo->condicao = novo->corpo = NULL;
    novo->proximoIrmao = NULL;
    return novo;
}

NoAST *criarNoAtribuicao(NoAST *id, NoAST *expressao) {
    NoAST *novo = malloc(sizeof(NoAST));
    novo->tipo = NO_ATRIBUICAO;
    novo->operador = '=';
    novo->esquerda = id;
    novo->direita = expressao;
    novo->condicao = novo->corpo = NULL;
    novo->proximoIrmao = NULL;
    return novo;
}

NoAST *criarNoIf(NoAST *condicao, NoAST *entao, NoAST *senao) {
    NoAST *novo = malloc(sizeof(NoAST));
    novo->tipo = NO_IF;
    novo->condicao = condicao;
    novo->esquerda = entao;    // Bloco "então"
    novo->direita = senao;     // Bloco "senão" (pode ser NULL)
    novo->corpo = NULL;
    novo->proximoIrmao = NULL;
    return novo;
}

NoAST *criarNoWhile(NoAST *condicao, NoAST *corpo) {
    NoAST *novo = malloc(sizeof(NoAST));
    novo->tipo = NO_WHILE;
    novo->condicao = condicao;
    novo->corpo = corpo;
    novo->esquerda = novo->direita = NULL;
    novo->proximoIrmao = NULL;
    return novo;
}

NoAST *criarNoDeclaracao(char *tipo, char *nome, NoAST *inicializacao) {
    NoAST *novo = malloc(sizeof(NoAST));
    novo->tipo = NO_DECLARACAO;
    strcpy(novo->nome, nome);
    // Tipo é armazenado no operador para simplicidade
    // Poderia ser melhorado com um campo específico para tipo
    novo->operador = tipo[0]; // 'i' para int, 'f' para float, etc.
    novo->direita = inicializacao;  // Expressão de inicialização (pode ser NULL)
    novo->esquerda = novo->condicao = novo->corpo = NULL;
    novo->proximoIrmao = NULL;
    return novo;
}

NoAST *criarNoFuncao(char *tipo, char *nome, NoAST *parametros, NoAST *corpo) {
    NoAST *novo = malloc(sizeof(NoAST));
    novo->tipo = NO_FUNCAO;
    strcpy(novo->nome, nome);
    novo->operador = tipo[0];  // Tipo de retorno
    novo->esquerda = parametros;  // Lista de parâmetros
    novo->corpo = corpo;         // Bloco de código
    novo->direita = novo->condicao = NULL;
    novo->proximoIrmao = NULL;
    return novo;
}

NoAST *criarNoChamada(char *nome, NoAST *argumentos) {
    NoAST *novo = malloc(sizeof(NoAST));
    novo->tipo = NO_CHAMADA;
    strcpy(novo->nome, nome);
    novo->esquerda = argumentos;  // Lista de argumentos
    novo->direita = novo->condicao = novo->corpo = NULL;
    novo->proximoIrmao = NULL;
    return novo;
}

NoAST *criarNoBloco(NoAST *declaracoes) {
    NoAST *novo = malloc(sizeof(NoAST));
    novo->tipo = NO_BLOCO;
    novo->esquerda = declaracoes;  // Primeiro elemento da lista
    novo->direita = novo->condicao = novo->corpo = NULL;
    novo->proximoIrmao = NULL;
    return novo;
}

NoAST *adicionarIrmao(NoAST *node, NoAST *irmao) {
    if (!node) return irmao;
    
    NoAST *temp = node;
    while (temp->proximoIrmao != NULL) {
        temp = temp->proximoIrmao;
    }
    temp->proximoIrmao = irmao;
    return node;
}

void imprimirAST(NoAST *raiz) {
    if (raiz) {
        switch (raiz->tipo) {
            case NO_NUMERO:
                printf("%d", raiz->valor);
                break;
            case NO_ID:
                printf("%s", raiz->nome);
                break;
            case NO_OPERADOR:
                printf("(");
                imprimirAST(raiz->esquerda);
                printf(" %c ", raiz->operador);
                imprimirAST(raiz->direita);
                printf(")");
                break;
            case NO_ATRIBUICAO:
                imprimirAST(raiz->esquerda);
                printf(" = ");
                imprimirAST(raiz->direita);
                break;
            case NO_IF:
                printf("if (");
                imprimirAST(raiz->condicao);
                printf(") ");
                imprimirAST(raiz->esquerda);
                if (raiz->direita) {
                    printf(" else ");
                    imprimirAST(raiz->direita);
                }
                break;
            case NO_WHILE:
                printf("while (");
                imprimirAST(raiz->condicao);
                printf(") ");
                imprimirAST(raiz->corpo);
                break;
            case NO_DECLARACAO:
                printf("%c %s", raiz->operador, raiz->nome);
                if (raiz->direita) {
                    printf(" = ");
                    imprimirAST(raiz->direita);
                }
                break;
            case NO_FUNCAO:
                printf("%c %s(", raiz->operador, raiz->nome);
                imprimirAST(raiz->esquerda);
                printf(") ");
                imprimirAST(raiz->corpo);
                break;
            case NO_CHAMADA:
                printf("%s(", raiz->nome);
                imprimirAST(raiz->esquerda);
                printf(")");
                break;
            case NO_BLOCO:
                printf("{ ");
                imprimirAST(raiz->esquerda);
                printf(" }");
                break;
        }
        
        // Imprimir irmãos (para listas)
        if (raiz->proximoIrmao) {
            printf(", ");
            imprimirAST(raiz->proximoIrmao);
        }
    }
}

void imprimirASTDetalhada(NoAST *raiz, int nivel) {
    if (!raiz) return;
    
    // Indentação
    for (int i = 0; i < nivel; i++) {
        printf("  ");
    }
    
    // Imprime informações do nó atual
    switch (raiz->tipo) {
        case NO_NUMERO:
            printf("Número: %d\n", raiz->valor);
            break;
        case NO_ID:
            printf("Identificador: %s\n", raiz->nome);
            break;
        case NO_OPERADOR:
            printf("Operador: %c\n", raiz->operador);
            break;
        case NO_ATRIBUICAO:
            printf("Atribuição:\n");
            break;
        case NO_IF:
            printf("If:\n");
            break;
        case NO_WHILE:
            printf("While:\n");
            break;
        case NO_DECLARACAO:
            printf("Declaração: %c %s\n", raiz->operador, raiz->nome);
            break;
        case NO_FUNCAO:
            printf("Função: %c %s\n", raiz->operador, raiz->nome);
            break;
        case NO_CHAMADA:
            printf("Chamada: %s\n", raiz->nome);
            break;
        case NO_BLOCO:
            printf("Bloco:\n");
            break;
    }
    
    // Visita filhos recursivamente
    if (raiz->esquerda)
        imprimirASTDetalhada(raiz->esquerda, nivel + 1);
    if (raiz->direita)
        imprimirASTDetalhada(raiz->direita, nivel + 1);
    if (raiz->condicao)
        imprimirASTDetalhada(raiz->condicao, nivel + 1);
    if (raiz->corpo)
        imprimirASTDetalhada(raiz->corpo, nivel + 1);
    
    // Visita irmãos no mesmo nível
    if (raiz->proximoIrmao)
        imprimirASTDetalhada(raiz->proximoIrmao, nivel);
}

void liberarAST(NoAST *raiz) {
    if (!raiz) return;
    
    // Libera todos os nós recursivamente
    liberarAST(raiz->esquerda);
    liberarAST(raiz->direita);
    liberarAST(raiz->condicao);
    liberarAST(raiz->corpo);
    liberarAST(raiz->proximoIrmao);
    
    free(raiz);
}

// Função auxiliar para gerar o tipo C a partir do operador usado na AST
char* obterTipoC(char operador) {
    switch(operador) {
        case 'i': return "int";
        case 'f': return "float";
        case 'b': return "bool";
        case 'v': return "void";
        default: return "int";  // Tipo padrão
    }
}

// Função para gerar código a partir dos parâmetros de função
void gerarParametros(NoAST *param, FILE *saida) {
    if (!param) return;
    
    // Gera o tipo e o nome do parâmetro
    fprintf(saida, "%s %s", obterTipoC(param->operador), param->nome);
    
    // Se houver mais parâmetros, adicione uma vírgula e continue
    if (param->proximoIrmao) {
        fprintf(saida, ", ");
        gerarParametros(param->proximoIrmao, saida);
    }
}

// Função para gerar código a partir dos argumentos de chamada de função
void gerarArgumentos(NoAST *arg, FILE *saida) {
    if (!arg) return;
    
    // Gera o código para o argumento atual
    gerarCodigoC(arg, saida);
    
    // Se houver mais argumentos, adicione uma vírgula e continue
    if (arg->proximoIrmao) {
        fprintf(saida, ", ");
        gerarArgumentos(arg->proximoIrmao, saida);
    }
}

// Gera código C a partir da AST
void gerarCodigoC(NoAST *raiz, FILE *saida) {
    if (!raiz) return;
    
    switch(raiz->tipo) {
        case NO_NUMERO:
            fprintf(saida, "%d", raiz->valor);
            break;
            
        case NO_ID:
            fprintf(saida, "%s", raiz->nome);
            break;
            
        case NO_OPERADOR:
            if (raiz->operador == 'r') {  // Return
                if (raiz->esquerda) {
                    fprintf(saida, "return ");
                    gerarCodigoC(raiz->esquerda, saida);
                    fprintf(saida, ";\n");
                } else {
                    fprintf(saida, "return;\n");
                }
            } else {
                fprintf(saida, "(");
                gerarCodigoC(raiz->esquerda, saida);
                
                // Traduzir operadores
                switch(raiz->operador) {
                    case '+': fprintf(saida, " + "); break;
                    case '-': fprintf(saida, " - "); break;
                    case '*': fprintf(saida, " * "); break;
                    case '/': fprintf(saida, " / "); break;
                    case '<': fprintf(saida, " < "); break;
                    case '>': fprintf(saida, " > "); break;
                    case 'L': fprintf(saida, " <= "); break;  // L para '≤'
                    case 'G': fprintf(saida, " >= "); break;  // G para '≥'
                    case '=': fprintf(saida, " == "); break;  // = para '=='
                    case '!': fprintf(saida, " != "); break;  // ! para '!='
                    default:  fprintf(saida, " %c ", raiz->operador);
                }
                
                gerarCodigoC(raiz->direita, saida);
                fprintf(saida, ")");
            }
            break;
            
        case NO_ATRIBUICAO:
            // Para atribuições, primeiro verifica se é a primeira vez que a variável é usada
            // Se for, adiciona o tipo antes
            {
                char *nome = raiz->esquerda->nome;
                Simbolo *s = buscarSimbolo(nome);
                if (s) {
                    // Variável já existe, simplesmente atribui
                    gerarCodigoC(raiz->esquerda, saida);
                    fprintf(saida, " = ");
                    gerarCodigoC(raiz->direita, saida);
                    fprintf(saida, ";\n");
                } else {
                    // Nova variável, infere o tipo com base no valor atribuído
                    fprintf(saida, "int ");  // Tipo padrão, poderia ser melhorado com inferência de tipo
                    gerarCodigoC(raiz->esquerda, saida);
                    fprintf(saida, " = ");
                    gerarCodigoC(raiz->direita, saida);
                    fprintf(saida, ";\n");
                    
                    // Adiciona à tabela de símbolos
                    inserirSimbolo(nome, "int");  // Tipo padrão
                }
            }
            break;
            
        case NO_IF:
            fprintf(saida, "if (");
            gerarCodigoC(raiz->condicao, saida);
            fprintf(saida, ") {\n");
            gerarCodigoC(raiz->esquerda, saida);  // Bloco "então"
            fprintf(saida, "}\n");
            
            if (raiz->direita) {  // Bloco "senão"
                fprintf(saida, "else {\n");
                gerarCodigoC(raiz->direita, saida);
                fprintf(saida, "}\n");
            }
            break;
            
        case NO_WHILE:
            fprintf(saida, "while (");
            gerarCodigoC(raiz->condicao, saida);
            fprintf(saida, ") {\n");
            gerarCodigoC(raiz->corpo, saida);
            fprintf(saida, "}\n");
            break;
            
        case NO_DECLARACAO:
            fprintf(saida, "%s %s", obterTipoC(raiz->operador), raiz->nome);
            
            if (raiz->direita) {  // Se tem inicialização
                fprintf(saida, " = ");
                gerarCodigoC(raiz->direita, saida);
            }
            
            fprintf(saida, ";\n");
            break;
            
        case NO_FUNCAO:
            // Adicionar à tabela de símbolos
            inserirSimbolo(raiz->nome, "function");
            
            // Gerar cabeçalho da função
            fprintf(saida, "%s %s(", obterTipoC(raiz->operador), raiz->nome);
            
            // Gerar parâmetros
            gerarParametros(raiz->esquerda, saida);
            
            // Gerar corpo da função
            fprintf(saida, ") {\n");
            gerarCodigoC(raiz->corpo, saida);
            fprintf(saida, "}\n\n");
            break;
            
        case NO_CHAMADA:
            fprintf(saida, "%s(", raiz->nome);
            gerarArgumentos(raiz->esquerda, saida);
            fprintf(saida, ")");
            break;
            
        case NO_BLOCO:
            gerarCodigoC(raiz->esquerda, saida);
            break;
    }
    
    // Processa nós irmãos
    if (raiz->proximoIrmao) {
        gerarCodigoC(raiz->proximoIrmao, saida);
    }
}