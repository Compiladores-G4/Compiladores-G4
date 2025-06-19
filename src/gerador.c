#include "gerador.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Inicializa a estrutura de código intermediário
CodigoIntermediario* inicializarCodigoIntermediario() {
    CodigoIntermediario *codigo = (CodigoIntermediario*)malloc(sizeof(CodigoIntermediario));
    if (codigo == NULL) {
        fprintf(stderr, "Erro: Falha na alocação de memória para código intermediário\n");
        exit(1);
    }
    
    codigo->inicio = NULL;
    codigo->fim = NULL;
    codigo->numTemporarios = 0;
    codigo->numRotulos = 0;
    
    return codigo;
}

// Libera a memória alocada para o código intermediário
void liberarCodigoIntermediario(CodigoIntermediario *codigo) {
    if (codigo == NULL) return;
    
    Instrucao *atual = codigo->inicio;
    while (atual != NULL) {
        Instrucao *temp = atual;
        atual = atual->prox;
        free(temp);
    }
    
    free(codigo);
}

// Adiciona uma nova instrução ao código intermediário
void adicionarInstrucao(CodigoIntermediario *codigo, TipoOperacao op, 
                        const char *resultado, const char *arg1, const char *arg2, int rotulo) {
    Instrucao *nova = (Instrucao*)malloc(sizeof(Instrucao));
    if (nova == NULL) {
        fprintf(stderr, "Erro: Falha na alocação de memória para instrução\n");
        exit(1);
    }
    
    nova->op = op;
    
    if (resultado != NULL) strcpy(nova->resultado, resultado);
    else nova->resultado[0] = '\0';
    
    if (arg1 != NULL) strcpy(nova->arg1, arg1);
    else nova->arg1[0] = '\0';
    
    if (arg2 != NULL) strcpy(nova->arg2, arg2);
    else nova->arg2[0] = '\0';
    
    nova->rotulo = rotulo;
    nova->prox = NULL;
    
    // Adiciona a instrução à lista
    if (codigo->inicio == NULL) {
        codigo->inicio = nova;
        codigo->fim = nova;
    } else {
        codigo->fim->prox = nova;
        codigo->fim = nova;
    }
}

// Gera um novo nome de variável temporária
char* gerarTemporario(CodigoIntermediario *codigo) {
    static char temp[16];
    sprintf(temp, "t%d", codigo->numTemporarios++);
    return strdup(temp);
}

// Gera um novo rótulo para saltos
int gerarRotulo(CodigoIntermediario *codigo) {
    return codigo->numRotulos++;
}

// Converte operador da AST para operador de código intermediário
TipoOperacao mapearOperador(char op) {
    switch(op) {
        case '+': return OP_ADD;
        case '-': return OP_SUB;
        case '*': return OP_MUL;
        case '/': return OP_DIV;
        case '=': return OP_EQ;
        case '!': return OP_NE;
        case '<': return OP_LT;
        case '>': return OP_GT;
        case 'l': return OP_LE;  // <=
        case 'g': return OP_GE;  // >=
        case '&': return OP_AND; // and
        case '|': return OP_OR;  // or
        case '~': return OP_NOT; // not
        default: return OP_ASSIGN;
    }
}

// Gera código para uma expressão
void gerarCodigoExpressao(CodigoIntermediario *codigo, NoAST *no, char *resultado) {
    if (no == NULL) return;
    
    char *temp1;
    char *temp2;
    char valorStr[64];
    
    switch(no->tipo) {
        case NO_NUMERO:
            sprintf(valorStr, "%d", no->valor);
            adicionarInstrucao(codigo, OP_ASSIGN, resultado, valorStr, NULL, -1);
            break;
            
        case NO_ID:
            adicionarInstrucao(codigo, OP_LOAD, resultado, no->nome, NULL, -1);
            break;
            
        case NO_OPERADOR:
            // Operadores especiais como True(T) e False(F)
            if (no->operador == 'T') {
                adicionarInstrucao(codigo, OP_ASSIGN, resultado, "1", NULL, -1);
                return;
            } else if (no->operador == 'F') {
                adicionarInstrucao(codigo, OP_ASSIGN, resultado, "0", NULL, -1);
                return;
            } else if (no->operador == 'f') { // Float
                // Assumindo que 0.0 é representado como "f" na AST
                adicionarInstrucao(codigo, OP_ASSIGN, resultado, "0.0", NULL, -1);
                return;
            } else if (no->operador == '~') { // NOT (operador unário)
                char *temp1 = gerarTemporario(codigo);
                gerarCodigoExpressao(codigo, no->esquerda, temp1);
                adicionarInstrucao(codigo, OP_NOT, resultado, temp1, NULL, -1);
                free(temp1);
                return;
            }
            
            temp1 = gerarTemporario(codigo);
            temp2 = gerarTemporario(codigo);
            
            gerarCodigoExpressao(codigo, no->esquerda, temp1);
            gerarCodigoExpressao(codigo, no->direita, temp2);
            
            TipoOperacao op = mapearOperador(no->operador);
            adicionarInstrucao(codigo, op, resultado, temp1, temp2, -1);
            
            free(temp1);
            free(temp2);
            break;
            
        case NO_CHAMADA:
            gerarCodigoChamada(codigo, no, resultado);
            break;
            
        default:
            fprintf(stderr, "Erro: Tipo de nó não suportado para expressão\n");
            break;
    }
}

// Gera código para uma declaração
void gerarCodigoDeclaracao(CodigoIntermediario *codigo, NoAST *no) {
    if (no == NULL) return;
    
    if (no->direita) {
        char *temp = gerarTemporario(codigo);
        gerarCodigoExpressao(codigo, no->direita, temp);
        adicionarInstrucao(codigo, OP_STORE, no->nome, temp, NULL, -1);
        free(temp);
    }
}

// Gera código para uma atribuição
void gerarCodigoAtribuicao(CodigoIntermediario *codigo, NoAST *no) {
    if (no == NULL) return;
    
    NoAST *id = no->esquerda;
    NoAST *expr = no->direita;
    
    char *temp = gerarTemporario(codigo);
    gerarCodigoExpressao(codigo, expr, temp);
    adicionarInstrucao(codigo, OP_STORE, id->nome, temp, NULL, -1);
    free(temp);
}

// Gera código para uma estrutura condicional (if-else)
void gerarCodigoCondicional(CodigoIntermediario *codigo, NoAST *no) {
    if (no == NULL) return;
    
    char *condicao = gerarTemporario(codigo);
    int rotuloFalso = gerarRotulo(codigo);
    int rotuloFim = gerarRotulo(codigo);
    
    // Gera código para a condição
    gerarCodigoExpressao(codigo, no->condicao, condicao);
    
    // Salto condicional: se condição falsa, vai para 'rotuloFalso'
    adicionarInstrucao(codigo, OP_CJUMP, NULL, condicao, "0", rotuloFalso);
    
    // Código para o bloco 'then'
    NoAST *atual = no->esquerda;
    while (atual != NULL) {
        switch (atual->tipo) {
            case NO_ATRIBUICAO:
                gerarCodigoAtribuicao(codigo, atual);
                break;
            case NO_DECLARACAO:
                gerarCodigoDeclaracao(codigo, atual);
                break;
            case NO_IF:
                gerarCodigoCondicional(codigo, atual);
                break;
            case NO_WHILE:
                gerarCodigoLaco(codigo, atual);
                break;
            default:
                // Tratar outros tipos se necessário
                break;
        }
        atual = atual->proximoIrmao;
    }
    
    // Salto para o fim do if-else
    adicionarInstrucao(codigo, OP_JUMP, NULL, NULL, NULL, rotuloFim);
    
    // Rótulo para o bloco 'else'
    adicionarInstrucao(codigo, OP_LABEL, NULL, NULL, NULL, rotuloFalso);
    
    // Código para o bloco 'else' se existir
    if (no->direita) {
        atual = no->direita;
        while (atual != NULL) {
            switch (atual->tipo) {
                case NO_ATRIBUICAO:
                    gerarCodigoAtribuicao(codigo, atual);
                    break;
                case NO_DECLARACAO:
                    gerarCodigoDeclaracao(codigo, atual);
                    break;
                case NO_IF:
                    gerarCodigoCondicional(codigo, atual);
                    break;
                case NO_WHILE:
                    gerarCodigoLaco(codigo, atual);
                    break;
                default:
                    // Tratar outros tipos se necessário
                    break;
            }
            atual = atual->proximoIrmao;
        }
    }
    
    // Rótulo para o fim do if-else
    adicionarInstrucao(codigo, OP_LABEL, NULL, NULL, NULL, rotuloFim);
    
    free(condicao);
}

// Gera código para uma estrutura de laço (while)
void gerarCodigoLaco(CodigoIntermediario *codigo, NoAST *no) {
    if (no == NULL) return;
    
    int rotuloInicio = gerarRotulo(codigo);
    int rotuloFim = gerarRotulo(codigo);
    char *condicao = gerarTemporario(codigo);
    
    // Rótulo de início do laço
    adicionarInstrucao(codigo, OP_LABEL, NULL, NULL, NULL, rotuloInicio);
    
    // Gera código para a condição
    gerarCodigoExpressao(codigo, no->condicao, condicao);
    
    // Salto condicional: se condição falsa, vai para 'rotuloFim'
    adicionarInstrucao(codigo, OP_CJUMP, NULL, condicao, "0", rotuloFim);
    
    // Código para o corpo do laço
    NoAST *atual = no->corpo;
    while (atual != NULL) {
        switch (atual->tipo) {
            case NO_ATRIBUICAO:
                gerarCodigoAtribuicao(codigo, atual);
                break;
            case NO_DECLARACAO:
                gerarCodigoDeclaracao(codigo, atual);
                break;
            case NO_IF:
                gerarCodigoCondicional(codigo, atual);
                break;
            case NO_WHILE:
                gerarCodigoLaco(codigo, atual);
                break;
            default:
                // Tratar outros tipos se necessário
                break;
        }
        atual = atual->proximoIrmao;
    }
    
    // Salto incondicional de volta para o início do laço
    adicionarInstrucao(codigo, OP_JUMP, NULL, NULL, NULL, rotuloInicio);
    
    // Rótulo para o fim do laço
    adicionarInstrucao(codigo, OP_LABEL, NULL, NULL, NULL, rotuloFim);
    
    free(condicao);
}

// Gera código para uma definição de função
void gerarCodigoFuncao(CodigoIntermediario *codigo, NoAST *no) {
    if (no == NULL) return;
    
    // Gera rótulo e instrução para início da função
    int rotuloFuncao = gerarRotulo(codigo);
    adicionarInstrucao(codigo, OP_LABEL, NULL, NULL, NULL, rotuloFuncao);
    
    // Código especial para marcar início de função
    char funcName[64];
    char nomeFuncao[40]; // Reduced size to fit in sprintf with suffixes
    strncpy(nomeFuncao, no->nome, sizeof(nomeFuncao) - 1);
    nomeFuncao[sizeof(nomeFuncao) - 1] = '\0'; // Ensure null termination
    sprintf(funcName, "FUNC_%s_BEGIN", nomeFuncao);
    adicionarInstrucao(codigo, OP_LABEL, funcName, NULL, NULL, -1);
    
    // Processar parâmetros da função
    NoAST *param = no->esquerda;
    int paramCount = 0;
    while (param != NULL) {
        if (param->tipo == NO_DECLARACAO) {
            char paramName[64];
            sprintf(paramName, "param_%s", param->nome);
            adicionarInstrucao(codigo, OP_PARAM, param->nome, paramName, NULL, -1);
            paramCount++;
        }
        param = param->proximoIrmao;
    }
    
    // Gera código para o corpo da função
    NoAST *atual = no->corpo;
    while (atual != NULL) {
        switch (atual->tipo) {
            case NO_ATRIBUICAO:
                gerarCodigoAtribuicao(codigo, atual);
                break;
            case NO_DECLARACAO:
                gerarCodigoDeclaracao(codigo, atual);
                break;
            case NO_IF:
                gerarCodigoCondicional(codigo, atual);
                break;
            case NO_WHILE:
                gerarCodigoLaco(codigo, atual);
                break;
            case NO_OPERADOR:
                if (atual->operador == 'r') {  // Return
                    if (atual->esquerda) {
                        char *temp = gerarTemporario(codigo);
                        gerarCodigoExpressao(codigo, atual->esquerda, temp);
                        adicionarInstrucao(codigo, OP_RETURN, NULL, temp, NULL, -1);
                        free(temp);
                    } else {
                        // Return sem valor
                        adicionarInstrucao(codigo, OP_RETURN, NULL, NULL, NULL, -1);
                    }
                }
                break;
            default:
                // Outros tipos se necessário
                break;
        }
        atual = atual->proximoIrmao;
    }
    
    // Código especial para marcar fim de função
    sprintf(funcName, "FUNC_%s_END", nomeFuncao);
    adicionarInstrucao(codigo, OP_LABEL, funcName, NULL, NULL, -1);
}

// Gera código para uma chamada de função
void gerarCodigoChamada(CodigoIntermediario *codigo, NoAST *no, char *resultado) {
    if (no == NULL) return;
    
    // Processar argumentos da chamada
    NoAST *arg = no->esquerda;
    int numArgs = 0;
    
    while (arg != NULL) {
        char *temp = gerarTemporario(codigo);
        gerarCodigoExpressao(codigo, arg, temp);
        
        char paramName[64];
        sprintf(paramName, "param%d", numArgs++);
        adicionarInstrucao(codigo, OP_PARAM, paramName, temp, NULL, -1);
        
        free(temp);
        arg = arg->proximoIrmao;
    }
    
    // Instrução de chamada de função
    adicionarInstrucao(codigo, OP_CALL, resultado, no->nome, NULL, -1);
}

// Função principal para geração de código intermediário
CodigoIntermediario* gerarCodigoIntermediario(NoAST *raiz) {
    if (raiz == NULL) return NULL;
    
    CodigoIntermediario *codigo = inicializarCodigoIntermediario();
    
    // Inicia o código com um rótulo principal
    adicionarInstrucao(codigo, OP_LABEL, "main", NULL, NULL, -1);
    
    // Percorre a árvore e gera código para cada nó
    NoAST *atual = raiz;
    while (atual != NULL) {
        switch (atual->tipo) {
            case NO_ATRIBUICAO:
                gerarCodigoAtribuicao(codigo, atual);
                break;
            case NO_DECLARACAO:
                gerarCodigoDeclaracao(codigo, atual);
                break;
            case NO_IF:
                gerarCodigoCondicional(codigo, atual);
                break;
            case NO_WHILE:
                gerarCodigoLaco(codigo, atual);
                break;
            case NO_FUNCAO:
                gerarCodigoFuncao(codigo, atual);
                break;
            default:
                // Outros casos se necessário
                break;
        }
        atual = atual->proximoIrmao;
    }
    
    // Finaliza o código com uma instrução especial
    adicionarInstrucao(codigo, OP_LABEL, "end", NULL, NULL, -1);
    
    return codigo;
}

// Imprime o código intermediário gerado
void imprimirCodigoIntermediario(CodigoIntermediario *codigo) {
    if (codigo == NULL) return;
    
    printf("\n===== CÓDIGO INTERMEDIÁRIO =====\n\n");
    
    Instrucao *atual = codigo->inicio;
    while (atual != NULL) {
        // Imprime rótulo se houver
        if (atual->op == OP_LABEL) {
            if (atual->rotulo >= 0) {
                printf("L%d:\n", atual->rotulo);
            } else if (atual->resultado[0] != '\0') {
                printf("%s:\n", atual->resultado);
            }
        } else {
            // Indenta instruções normais
            printf("    ");
            
            // Imprime instrução de acordo com o tipo
            switch(atual->op) {
                case OP_ADD:
                    printf("%s = %s + %s\n", atual->resultado, atual->arg1, atual->arg2);
                    break;
                case OP_SUB:
                    printf("%s = %s - %s\n", atual->resultado, atual->arg1, atual->arg2);
                    break;
                case OP_MUL:
                    printf("%s = %s * %s\n", atual->resultado, atual->arg1, atual->arg2);
                    break;
                case OP_DIV:
                    printf("%s = %s / %s\n", atual->resultado, atual->arg1, atual->arg2);
                    break;
                case OP_ASSIGN:
                    printf("%s = %s\n", atual->resultado, atual->arg1);
                    break;
                case OP_JUMP:
                    printf("goto L%d\n", atual->rotulo);
                    break;
                case OP_CJUMP:
                    if (strcmp(atual->arg2, "0") == 0) {
                        printf("if %s == 0 goto L%d\n", atual->arg1, atual->rotulo);
                    } else {
                        printf("if %s != %s goto L%d\n", atual->arg1, atual->arg2, atual->rotulo);
                    }
                    break;
                case OP_LT:
                    printf("%s = %s < %s\n", atual->resultado, atual->arg1, atual->arg2);
                    break;
                case OP_GT:
                    printf("%s = %s > %s\n", atual->resultado, atual->arg1, atual->arg2);
                    break;
                case OP_LE:
                    printf("%s = %s <= %s\n", atual->resultado, atual->arg1, atual->arg2);
                    break;
                case OP_GE:
                    printf("%s = %s >= %s\n", atual->resultado, atual->arg1, atual->arg2);
                    break;
                case OP_EQ:
                    printf("%s = %s == %s\n", atual->resultado, atual->arg1, atual->arg2);
                    break;
                case OP_NE:
                    printf("%s = %s != %s\n", atual->resultado, atual->arg1, atual->arg2);
                    break;
                case OP_AND:
                    printf("%s = %s && %s\n", atual->resultado, atual->arg1, atual->arg2);
                    break;
                case OP_OR:
                    printf("%s = %s || %s\n", atual->resultado, atual->arg1, atual->arg2);
                    break;
                case OP_NOT:
                    printf("%s = !%s\n", atual->resultado, atual->arg1);
                    break;
                case OP_PARAM:
                    printf("param %s\n", atual->arg1);
                    break;
                case OP_CALL:
                    if (atual->resultado[0] != '\0') {
                        printf("%s = call %s\n", atual->resultado, atual->arg1);
                    } else {
                        printf("call %s\n", atual->arg1);
                    }
                    break;
                case OP_RETURN:
                    if (atual->arg1[0] != '\0') {
                        printf("return %s\n", atual->arg1);
                    } else {
                        printf("return\n");
                    }
                    break;
                case OP_LOAD:
                    printf("%s = %s\n", atual->resultado, atual->arg1);
                    break;
                case OP_STORE:
                    printf("%s = %s\n", atual->resultado, atual->arg1);
                    break;
                default:
                    printf("Instrução não reconhecida\n");
                    break;
            }
        }
        
        atual = atual->prox;
    }
    
    printf("\n================================\n");
}
