#include "otimizador.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

TabelaUsoVariaveis* criarTabelaUsoVariaveis() {
    TabelaUsoVariaveis *tabela = (TabelaUsoVariaveis*)malloc(sizeof(TabelaUsoVariaveis));
    if (tabela == NULL) {
        fprintf(stderr, "Erro: Falha na alocação de memória para tabela de uso de variáveis\n");
        exit(1);
    }
    
    tabela->capacidade = 50; 
    tabela->tamanho = 0;
    tabela->variaveis = (VariavelUso*)malloc(tabela->capacidade * sizeof(VariavelUso));
    
    if (tabela->variaveis == NULL) {
        fprintf(stderr, "Erro: Falha na alocação de memória para variáveis\n");
        free(tabela);
        exit(1);
    }
    
    return tabela;
}

void liberarTabelaUsoVariaveis(TabelaUsoVariaveis *tabela) {
    if (tabela != NULL) {
        if (tabela->variaveis != NULL) {
            free(tabela->variaveis);
        }
        free(tabela);
    }
}

int ehTemporario(const char *nome) {
    if (nome == NULL || nome[0] == '\0') return 0;
    
    if (nome[0] == 't' && isdigit(nome[1])) {
        for (int i = 1; nome[i] != '\0'; i++) {
            if (!isdigit(nome[i])) return 0;
        }
        return 1;
    }
    return 0;
}

typedef struct {
    char nome[50];
    char valor[50];
    int ehConstante;
} ValorConstante;

typedef struct {
    ValorConstante *valores;
    int tamanho;
    int capacidade;
} TabelaConstantes;

TabelaConstantes* criarTabelaConstantes() {
    TabelaConstantes *tabela = (TabelaConstantes*)malloc(sizeof(TabelaConstantes));
    if (tabela == NULL) {
        fprintf(stderr, "Erro: Falha na alocação de memória para tabela de constantes\n");
        exit(1);
    }
    
    tabela->capacidade = 50;  
    tabela->tamanho = 0;
    tabela->valores = (ValorConstante*)malloc(tabela->capacidade * sizeof(ValorConstante));
    
    if (tabela->valores == NULL) {
        fprintf(stderr, "Erro: Falha na alocação de memória para valores constantes\n");
        free(tabela);
        exit(1);
    }
    
    return tabela;
}

void liberarTabelaConstantes(TabelaConstantes *tabela) {
    if (tabela != NULL) {
        if (tabela->valores != NULL) {
            free(tabela->valores);
        }
        free(tabela);
    }
}

void registrarConstante(TabelaConstantes *tabela, const char *nome, const char *valor) {
    if (tabela == NULL || nome == NULL || valor == NULL) return;
    
    for (int i = 0; i < tabela->tamanho; i++) {
        if (strcmp(tabela->valores[i].nome, nome) == 0) {
            strcpy(tabela->valores[i].valor, valor);
            tabela->valores[i].ehConstante = 1;
            return;
        }
    }
    
    if (tabela->tamanho >= tabela->capacidade) {
        tabela->capacidade *= 2;
        tabela->valores = (ValorConstante*)realloc(tabela->valores, 
                                                tabela->capacidade * sizeof(ValorConstante));
        if (tabela->valores == NULL) {
            fprintf(stderr, "Erro: Falha na realocação de memória para valores constantes\n");
            exit(1);
        }
    }
    
    strcpy(tabela->valores[tabela->tamanho].nome, nome);
    strcpy(tabela->valores[tabela->tamanho].valor, valor);
    tabela->valores[tabela->tamanho].ehConstante = 1;
    tabela->tamanho++;
}

const char* obterValorConstante(TabelaConstantes *tabela, const char *nome) {
    if (tabela == NULL || nome == NULL) return NULL;
    
    for (int i = 0; i < tabela->tamanho; i++) {
        if (strcmp(tabela->valores[i].nome, nome) == 0 && tabela->valores[i].ehConstante) {
            return tabela->valores[i].valor;
        }
    }
    
    return NULL;
}

int ehConstante(const char *nome) {
    if (nome == NULL || nome[0] == '\0') return 0;
    
    if (strcmp(nome, "true") == 0 || strcmp(nome, "false") == 0) {
        return 1;
    }

    char *endptr;
    strtod(nome, &endptr);
    
    return *endptr == '\0';
}

void marcarVariavelUsada(TabelaUsoVariaveis *tabela, const char *nome) {
    if (tabela == NULL || nome == NULL || nome[0] == '\0') return;

    if (ehConstante(nome)) return;
    
    for (int i = 0; i < tabela->tamanho; i++) {
        if (strcmp(tabela->variaveis[i].nome, nome) == 0) {
            tabela->variaveis[i].usada = 1;
            return;
        }
    }
    
    if (tabela->tamanho >= tabela->capacidade) {
        tabela->capacidade *= 2;
        tabela->variaveis = (VariavelUso*)realloc(tabela->variaveis, 
                                                tabela->capacidade * sizeof(VariavelUso));
        if (tabela->variaveis == NULL) {
            fprintf(stderr, "Erro: Falha na realocação de memória para variáveis\n");
            exit(1);
        }
    }
    
    strcpy(tabela->variaveis[tabela->tamanho].nome, nome);
    tabela->variaveis[tabela->tamanho].usada = 1;
    tabela->tamanho++;
}

int verificarVariavelUsada(TabelaUsoVariaveis *tabela, const char *nome) {
    if (tabela == NULL || nome == NULL || nome[0] == '\0') return 0;
    
    if (ehConstante(nome)) return 1;
    
    for (int i = 0; i < tabela->tamanho; i++) {
        if (strcmp(tabela->variaveis[i].nome, nome) == 0) {
            return tabela->variaveis[i].usada;
        }
    }
    
    return 0;  
}

void analisarUsoVariaveis(CodigoIntermediario *codigo, TabelaUsoVariaveis *tabela) {
    if (codigo == NULL || tabela == NULL) return;
    
    Instrucao *atual = codigo->inicio;
    while (atual != NULL) {
        if (atual->resultado[0] != '\0' && 
            atual->op != OP_LABEL && 
            atual->op != OP_JUMP && 
            atual->op != OP_CJUMP) {
            
            int encontrado = 0;
            for (int i = 0; i < tabela->tamanho; i++) {
                if (strcmp(tabela->variaveis[i].nome, atual->resultado) == 0) {
                    encontrado = 1;
                    break;
                }
            }
            
            if (!encontrado) {
                if (tabela->tamanho >= tabela->capacidade) {
                    tabela->capacidade *= 2;
                    tabela->variaveis = (VariavelUso*)realloc(tabela->variaveis, 
                                                            tabela->capacidade * sizeof(VariavelUso));
                    if (tabela->variaveis == NULL) {
                        fprintf(stderr, "Erro: Falha na realocação de memória para variáveis\n");
                        exit(1);
                    }
                }
                
                strcpy(tabela->variaveis[tabela->tamanho].nome, atual->resultado);
                tabela->variaveis[tabela->tamanho].usada = 0;
                tabela->tamanho++;
            }
        }
        
        atual = atual->prox;
    }
    
    atual = codigo->inicio;
    while (atual != NULL) {
        if (atual->arg1[0] != '\0') {
            marcarVariavelUsada(tabela, atual->arg1);
        }
        
        if (atual->arg2[0] != '\0') {
            marcarVariavelUsada(tabela, atual->arg2);
        }
        
        switch (atual->op) {
            case OP_RETURN:
                if (atual->arg1[0] != '\0') {
                    marcarVariavelUsada(tabela, atual->arg1);
                }
                break;
                
            case OP_STORE:
                marcarVariavelUsada(tabela, atual->resultado);
                break;
                
            case OP_PARAM:
                marcarVariavelUsada(tabela, atual->arg1);
                break;
                
            default:
                break;
        }
        
        atual = atual->prox;
    }
}

void removerCodigoMorto(CodigoIntermediario *codigo) {
    if (codigo == NULL || codigo->inicio == NULL) return;

    int removidas = 0;

    TabelaUsoVariaveis *tabela = criarTabelaUsoVariaveis();

    Instrucao *atual = codigo->inicio;
    while (atual != NULL) {
        if (atual->arg1[0] != '\0') {
            marcarVariavelUsada(tabela, atual->arg1);
        }
        if (atual->arg2[0] != '\0') {
            marcarVariavelUsada(tabela, atual->arg2);
        }

        if (atual->op == OP_CJUMP || atual->op == OP_JUMP) {
            marcarVariavelUsada(tabela, atual->resultado);
        }
        
        if (!ehTemporario(atual->resultado)) {
            marcarVariavelUsada(tabela, atual->resultado);
        }

        atual = atual->prox;
    }

    Instrucao *anterior = NULL;
    atual = codigo->inicio;

    while (atual != NULL) {
        int remover = 0;

        if (ehTemporario(atual->resultado) && !verificarVariavelUsada(tabela, atual->resultado)) {
            if (atual->op != OP_CALL) {
                remover = 1;
                removidas++;
            }
        }

        if (remover) {
            Instrucao *temp = atual;
            if (anterior == NULL) {
                codigo->inicio = atual->prox;
            } else {
                anterior->prox = atual->prox;
            }
            atual = atual->prox;
            free(temp);
        } else {
            anterior = atual;
            atual = atual->prox;
        }
    }

    liberarTabelaUsoVariaveis(tabela);
}

void simplificarExpressoes(CodigoIntermediario *codigo) {
    if (codigo == NULL) return;

    int simplificadas = 0;
    
    TabelaConstantes *tabela = criarTabelaConstantes();
    
    Instrucao *atual = codigo->inicio;
    while (atual != NULL) {
        if (atual->op == OP_ASSIGN && ehConstante(atual->arg1)) {
            registrarConstante(tabela, atual->resultado, atual->arg1);
        }
        atual = atual->prox;
    }
    
    atual = codigo->inicio;
    while (atual != NULL) {
        if (atual->op == OP_ASSIGN) {
            atual = atual->prox;
            continue;
        }
        
        const char *val1 = NULL;
        const char *val2 = NULL;
        
        if (ehConstante(atual->arg1)) {
            val1 = atual->arg1;
        } else {
            val1 = obterValorConstante(tabela, atual->arg1);
        }
        
        if (ehConstante(atual->arg2)) {
            val2 = atual->arg2;
        } else {
            val2 = obterValorConstante(tabela, atual->arg2);
        }
        
        if (val1 != NULL && val2 != NULL) {
            double num1 = atof(val1);
            double num2 = atof(val2);
            double resultado = 0;
            int calculado = 1; 

            switch (atual->op) {
                case OP_ADD: resultado = num1 + num2; break;
                case OP_SUB: resultado = num1 - num2; break;
                case OP_MUL: resultado = num1 * num2; break;
                case OP_DIV:
                    if (num2 != 0) {
                        resultado = num1 / num2;
                    } else {
                        calculado = 0;
                    }
                    break;
                case OP_LT: resultado = num1 < num2; break;
                case OP_GT: resultado = num1 > num2; break;
                case OP_LE: resultado = num1 <= num2; break;
                case OP_GE: resultado = num1 >= num2; break;
                case OP_EQ: resultado = num1 == num2; break;
                case OP_NE: resultado = num1 != num2; break;
                default:
                    calculado = 0; 
                    break;
            }

            if (calculado) {
                simplificadas++;
                
                atual->op = OP_ASSIGN;
                
                char valor[50];
                sprintf(valor, "%g", resultado);
                strcpy(atual->arg1, valor);
                
                atual->arg2[0] = '\0';
                
                registrarConstante(tabela, atual->resultado, valor);
            }
        }
        
        atual = atual->prox;
    }
    
    liberarTabelaConstantes(tabela);
}

CodigoIntermediario* otimizarCodigoIntermediario(CodigoIntermediario *codigo) {
    if (codigo == NULL) return NULL;

    printf("\n===== APLICANDO OTIMIZAÇÕES =====\n");

    int alterado = 1;
    while (alterado) {
        alterado = 0;

        int count_antes = 0;
        for (Instrucao *i = codigo->inicio; i != NULL; i = i->prox) count_antes++;

        simplificarExpressoes(codigo);
        removerCodigoMorto(codigo);

        int count_depois = 0;
        for (Instrucao *i = codigo->inicio; i != NULL; i = i->prox) count_depois++;

        if (count_antes != count_depois) {
            alterado = 1;
        }
    }

    printf("Otimizações concluídas.\n");
    printf("================================\n");

    return codigo;
}

