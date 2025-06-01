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
        case '%': return OP_MOD;
        case '=': return OP_EQ;
        case '!': return OP_NE;
        case '<': return OP_LT;
        case '>': return OP_GT;
        case 'l': return OP_LE;  // <=
        case 'g': return OP_GE;  // >=
        case 'a': return OP_AND; // and
        case 'o': return OP_OR;  // or
        case 'n': return OP_NOT; // not
        default: return OP_ASSIGN;
    }
}

// Gera código para uma expressão
void gerarCodigoExpressao(CodigoIntermediario *codigo, NoAST *no, char *resultado) {
    if (no == NULL) return;
    
    char *temp1;
    char *temp2;
    char valorStr[64];
    int rotuloVerdadeiro, rotuloFalso, rotuloFim;
    
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
                // Aqui poderia ser melhorado para usar o valor real do float
                adicionarInstrucao(codigo, OP_ASSIGN, resultado, "0.0", NULL, -1);
                return;
            } else if (no->operador == 'n') { // not
                temp1 = gerarTemporario(codigo);
                gerarCodigoExpressao(codigo, no->esquerda, temp1);
                adicionarInstrucao(codigo, OP_NOT, resultado, temp1, NULL, -1);
                free(temp1);
                return;
            } else if (no->operador == 'a' || no->operador == 'o') { // and/or - avaliação de curto-circuito
                rotuloVerdadeiro = gerarRotulo(codigo);
                rotuloFalso = gerarRotulo(codigo);
                rotuloFim = gerarRotulo(codigo);
                
                // Gera código para avaliação de curto-circuito
                if (no->operador == 'a') { // and
                    temp1 = gerarTemporario(codigo);
                    gerarCodigoExpressao(codigo, no->esquerda, temp1);
                    
                    // Se o primeiro operando for falso, o resultado é falso
                    adicionarInstrucao(codigo, OP_CJUMP, NULL, temp1, "0", rotuloFalso);
                    
                    // Avalia o segundo operando
                    gerarCodigoExpressao(codigo, no->direita, resultado);
                    adicionarInstrucao(codigo, OP_JUMP, NULL, NULL, NULL, rotuloFim);
                    
                    // Rótulo para resultado falso
                    adicionarInstrucao(codigo, OP_LABEL, NULL, NULL, NULL, rotuloFalso);
                    adicionarInstrucao(codigo, OP_ASSIGN, resultado, "0", NULL, -1);
                    
                    // Rótulo para fim da avaliação
                    adicionarInstrucao(codigo, OP_LABEL, NULL, NULL, NULL, rotuloFim);
                    free(temp1);
                } else { // or
                    temp1 = gerarTemporario(codigo);
                    gerarCodigoExpressao(codigo, no->esquerda, temp1);
                    
                    // Se o primeiro operando for verdadeiro, o resultado é verdadeiro
                    adicionarInstrucao(codigo, OP_CJUMP, NULL, temp1, "1", rotuloVerdadeiro);
                    
                    // Avalia o segundo operando
                    gerarCodigoExpressao(codigo, no->direita, resultado);
                    adicionarInstrucao(codigo, OP_JUMP, NULL, NULL, NULL, rotuloFim);
                    
                    // Rótulo para resultado verdadeiro
                    adicionarInstrucao(codigo, OP_LABEL, NULL, NULL, NULL, rotuloVerdadeiro);
                    adicionarInstrucao(codigo, OP_ASSIGN, resultado, "1", NULL, -1);
                    
                    // Rótulo para fim da avaliação
                    adicionarInstrucao(codigo, OP_LABEL, NULL, NULL, NULL, rotuloFim);
                    free(temp1);
                }
                return;
            }
            
            // Operadores binários normais
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

// Função auxiliar para gerar código para um bloco de código
void gerarCodigoBloco(CodigoIntermediario *codigo, NoAST *bloco) {
    NoAST *atual = bloco;
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
            case NO_FOR:
                gerarCodigoFor(codigo, atual);
                break;
            case NO_OPERADOR:
                if (atual->operador == 'r') {  // Return
                    char *temp = NULL;
                    if (atual->esquerda) {
                        temp = gerarTemporario(codigo);
                        gerarCodigoExpressao(codigo, atual->esquerda, temp);
                        adicionarInstrucao(codigo, OP_RETURN, NULL, temp, NULL, -1);
                        free(temp);
                    } else {
                        // Return sem valor
                        adicionarInstrucao(codigo, OP_RETURN, NULL, NULL, NULL, -1);
                    }
                }
                break;
            case NO_CHAMADA:
                {
                    char *temp = gerarTemporario(codigo);
                    gerarCodigoChamada(codigo, atual, temp);
                    free(temp);
                }
                break;
            default:
                // Outros tipos de nós podem ser tratados aqui
                break;
        }
        atual = atual->proximoIrmao;
    }
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
    gerarCodigoBloco(codigo, no->esquerda);
    
    // Salto para o fim do if-else
    adicionarInstrucao(codigo, OP_JUMP, NULL, NULL, NULL, rotuloFim);
    
    // Rótulo para o bloco 'else'
    adicionarInstrucao(codigo, OP_LABEL, NULL, NULL, NULL, rotuloFalso);
    
    // Código para o bloco 'else' se existir
    if (no->direita) {
        gerarCodigoBloco(codigo, no->direita);
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
    gerarCodigoBloco(codigo, no->corpo);
    
    // Salto incondicional de volta para o início do laço
    adicionarInstrucao(codigo, OP_JUMP, NULL, NULL, NULL, rotuloInicio);
    
    // Rótulo para o fim do laço
    adicionarInstrucao(codigo, OP_LABEL, NULL, NULL, NULL, rotuloFim);
    
    free(condicao);
}

// Gera código para uma estrutura de laço for
void gerarCodigoFor(CodigoIntermediario *codigo, NoAST *no) {
    if (no == NULL) return;
    
    // Estrutura esperada para o nó FOR:
    // variavel: variável de controle
    // esquerda: valor inicial
    // direita: valor final
    // condicao: passo (opcional, padrão 1)
    // corpo: corpo do laço
    
    int rotuloInicio = gerarRotulo(codigo);
    int rotuloFim = gerarRotulo(codigo);
    char *varControle = gerarTemporario(codigo);
    char *valorFinal = gerarTemporario(codigo);
    char *passo = gerarTemporario(codigo);
    char *condicao = gerarTemporario(codigo);
    
    // Inicialização da variável de controle
    if (no->esquerda) {
        gerarCodigoExpressao(codigo, no->esquerda, varControle);
        if (no->variavel && no->variavel->tipo == NO_ID) {
            adicionarInstrucao(codigo, OP_STORE, no->variavel->nome, varControle, NULL, -1);
        }
    } else {
        // Valor inicial padrão: 0
        adicionarInstrucao(codigo, OP_ASSIGN, varControle, "0", NULL, -1);
        if (no->variavel && no->variavel->tipo == NO_ID) {
            adicionarInstrucao(codigo, OP_STORE, no->variavel->nome, varControle, NULL, -1);
        }
    }
    
    // Valor final
    if (no->direita) {
        gerarCodigoExpressao(codigo, no->direita, valorFinal);
    } else {
        // Valor final padrão: 10
        adicionarInstrucao(codigo, OP_ASSIGN, valorFinal, "10", NULL, -1);
    }
    
    // Passo
    if (no->condicao) {
        gerarCodigoExpressao(codigo, no->condicao, passo);
    } else {
        // Passo padrão: 1
        adicionarInstrucao(codigo, OP_ASSIGN, passo, "1", NULL, -1);
    }
    
    // Rótulo de início do laço
    adicionarInstrucao(codigo, OP_LABEL, NULL, NULL, NULL, rotuloInicio);
    
    // Condição de continuação do laço: varControle < valorFinal
    adicionarInstrucao(codigo, OP_LT, condicao, varControle, valorFinal, -1);
    adicionarInstrucao(codigo, OP_CJUMP, NULL, condicao, "0", rotuloFim);
    
    // Corpo do laço
    gerarCodigoBloco(codigo, no->corpo);
    
    // Incremento da variável de controle
    adicionarInstrucao(codigo, OP_ADD, varControle, varControle, passo, -1);
    if (no->variavel && no->variavel->tipo == NO_ID) {
        adicionarInstrucao(codigo, OP_STORE, no->variavel->nome, varControle, NULL, -1);
    }
    
    // Salto de volta para o início do laço
    adicionarInstrucao(codigo, OP_JUMP, NULL, NULL, NULL, rotuloInicio);
    
    // Rótulo para o fim do laço
    adicionarInstrucao(codigo, OP_LABEL, NULL, NULL, NULL, rotuloFim);
    
    free(varControle);
    free(valorFinal);
    free(passo);
    free(condicao);
}

// Gera código para uma definição de função
void gerarCodigoFuncao(CodigoIntermediario *codigo, NoAST *no) {
    if (no == NULL) return;
    
    // Adiciona rótulo para a função
    adicionarInstrucao(codigo, OP_LABEL_FUNC, no->nome, NULL, NULL, -1);
    
    // Processa os parâmetros (se houver)
    if (no->parametros) {
        NoAST *param = no->parametros;
        int numParam = 0;
        while (param != NULL) {
            // Assume que cada parâmetro é um nó NO_ID
            if (param->tipo == NO_ID) {
                adicionarInstrucao(codigo, OP_PARAM, param->nome, NULL, NULL, numParam);
                
                // Se o parâmetro tem um tipo especificado, podemos adicionar uma instrução de verificação de tipo
                if (param->tipoVar && strlen(param->tipoVar) > 0) {
                    adicionarInstrucao(codigo, OP_CHECK_TYPE, param->nome, param->tipoVar, NULL, -1);
                }
            }
            param = param->proximoIrmao;
            numParam++;
        }
    }
    
    // Processa o corpo da função
    gerarCodigoBloco(codigo, no->corpo);
    
    // Adiciona um retorno padrão no final da função (caso não haja um explícito)
    adicionarInstrucao(codigo, OP_RETURN, NULL, NULL, NULL, -1);
}

// Gera código para uma chamada de função
void gerarCodigoChamada(CodigoIntermediario *codigo, NoAST *no, char *resultado) {
    if (no == NULL) return;
    
    // Processa os argumentos (se houver)
    if (no->argumentos) {
        NoAST *arg = no->argumentos;
        int numArg = 0;
        
        // Primeiro, contamos o número total de argumentos
        int totalArgs = 0;
        NoAST *contadorArg = no->argumentos;
        while (contadorArg != NULL) {
            totalArgs++;
            contadorArg = contadorArg->proximoIrmao;
        }
        
        // Adicionamos uma instrução para verificar o número de argumentos
        adicionarInstrucao(codigo, OP_CHECK_ARGS, no->nome, NULL, NULL, totalArgs);
        
        // Agora processamos cada argumento
        while (arg != NULL) {
            char *temp = gerarTemporario(codigo);
            gerarCodigoExpressao(codigo, arg, temp);
            
            // Adicionamos o argumento
            adicionarInstrucao(codigo, OP_ARG, NULL, temp, NULL, numArg);
            
            free(temp);
            arg = arg->proximoIrmao;
            numArg++;
        }
    } else {
        // Se não há argumentos, verificamos que a função não espera argumentos
        adicionarInstrucao(codigo, OP_CHECK_ARGS, no->nome, NULL, NULL, 0);
    }
    
    // Chamada da função
    adicionarInstrucao(codigo, OP_CALL, resultado, no->nome, NULL, -1);
}

// Gera código para a AST completa
void gerarCodigo(CodigoIntermediario *codigo, NoAST *raiz) {
    if (raiz == NULL) return;
    
    // Primeiro, geramos código para todas as funções
    NoAST *no = raiz;
    while (no != NULL) {
        if (no->tipo == NO_FUNCAO) {
            gerarCodigoFuncao(codigo, no);
        }
        no = no->proximoIrmao;
    }
    
    // Adicionamos um rótulo para o início do programa principal
    adicionarInstrucao(codigo, OP_LABEL_FUNC, "main", NULL, NULL, -1);
    
    // Depois, geramos código para o programa principal (tudo que não é função)
    no = raiz;
    while (no != NULL) {
        if (no->tipo != NO_FUNCAO) {
            switch (no->tipo) {
                case NO_DECLARACAO:
                    gerarCodigoDeclaracao(codigo, no);
                    break;
                    
                case NO_ATRIBUICAO:
                    gerarCodigoAtribuicao(codigo, no);
                    break;
                    
                case NO_IF:
                    gerarCodigoCondicional(codigo, no);
                    break;
                    
                case NO_WHILE:
                    gerarCodigoLaco(codigo, no);
                    break;
                    
                case NO_FOR:
                    gerarCodigoFor(codigo, no);
                    break;
                    
                case NO_CHAMADA:
                    {
                        char *temp = gerarTemporario(codigo);
                        gerarCodigoChamada(codigo, no, temp);
                        free(temp);
                    }
                    break;
                    
                case NO_OPERADOR:
                    if (no->operador == 'r') { // return no programa principal
                        char *temp = NULL;
                        if (no->esquerda) {
                            temp = gerarTemporario(codigo);
                            gerarCodigoExpressao(codigo, no->esquerda, temp);
                            adicionarInstrucao(codigo, OP_RETURN, NULL, temp, NULL, -1);
                            free(temp);
                        } else {
                            adicionarInstrucao(codigo, OP_RETURN, NULL, NULL, NULL, -1);
                        }
                    }
                    break;
                    
                default:
                    // Outros tipos de nós podem ser tratados aqui
                    break;
            }
        }
        
        no = no->proximoIrmao;
    }
    
    // Adicionamos um retorno padrão no final do programa principal
    adicionarInstrucao(codigo, OP_RETURN, NULL, "0", NULL, -1);
}

// Função principal para geração de código intermediário
CodigoIntermediario* gerarCodigoIntermediario(NoAST *raiz) {
    if (raiz == NULL) return NULL;
    
    CodigoIntermediario *codigo = inicializarCodigoIntermediario();
    
    // Gera o código usando a função gerarCodigo
    gerarCodigo(codigo, raiz);
    
    return codigo;
}

// Imprime o código intermediário gerado
void imprimirCodigoIntermediario(CodigoIntermediario *codigo) {
    if (codigo == NULL) return;
    
    printf("\n===== CÓDIGO INTERMEDIÁRIO =====\n\n");
    
    Instrucao *atual = codigo->inicio;
    while (atual != NULL) {
        // Imprime rótulo se houver
        if (atual->op == OP_LABEL || atual->op == OP_LABEL_FUNC) {
            if (atual->rotulo >= 0) {
                printf("L%d:\n", atual->rotulo);
            } else if (atual->resultado[0] != '\0') {
                if (atual->op == OP_LABEL_FUNC) {
                    printf("\nFUNC %s:\n", atual->resultado);
                } else {
                    printf("%s:\n", atual->resultado);
                }
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
                case OP_MOD:
                    printf("%s = %s %% %s\n", atual->resultado, atual->arg1, atual->arg2);
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
                case OP_PARAM:
                    printf("param %d: %s\n", atual->rotulo, atual->resultado);
                    break;
                case OP_ARG:
                    printf("arg %d: %s\n", atual->rotulo, atual->arg1);
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
                case OP_CHECK_TYPE:
                    printf("check_type %s is %s\n", atual->resultado, atual->arg1);
                    break;
                case OP_CHECK_ARGS:
                    printf("check_args %s expects %d args\n", atual->resultado, atual->rotulo);
                    break;
                default:
                    printf("Instrução não reconhecida (%d)\n", atual->op);
                    break;
            }
        }
        
        atual = atual->prox;
    }
    
    printf("\n================================\n");
}
