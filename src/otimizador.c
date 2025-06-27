#include "otimizador.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Funções para gerenciar a tabela de uso de variáveis
TabelaUsoVariaveis* criarTabelaUsoVariaveis() {
    TabelaUsoVariaveis *tabela = (TabelaUsoVariaveis*)malloc(sizeof(TabelaUsoVariaveis));
    if (tabela == NULL) {
        fprintf(stderr, "Erro: Falha na alocação de memória para tabela de uso de variáveis\n");
        exit(1);
    }
    
    tabela->capacidade = 50;  // Tamanho inicial
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

// Verifica se uma string é um temporário (começa com 't' seguido de dígitos)
int ehTemporario(const char *nome) {
    if (nome == NULL || nome[0] == '\0') return 0;
    
    if (nome[0] == 't' && isdigit(nome[1])) {
        // Verifica se todos os caracteres após 't' são dígitos
        for (int i = 1; nome[i] != '\0'; i++) {
            if (!isdigit(nome[i])) return 0;
        }
        return 1;
    }
    return 0;
}

// Estrutura para rastrear valores constantes
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

// Cria uma tabela de constantes
TabelaConstantes* criarTabelaConstantes() {
    TabelaConstantes *tabela = (TabelaConstantes*)malloc(sizeof(TabelaConstantes));
    if (tabela == NULL) {
        fprintf(stderr, "Erro: Falha na alocação de memória para tabela de constantes\n");
        exit(1);
    }
    
    tabela->capacidade = 50;  // Tamanho inicial
    tabela->tamanho = 0;
    tabela->valores = (ValorConstante*)malloc(tabela->capacidade * sizeof(ValorConstante));
    
    if (tabela->valores == NULL) {
        fprintf(stderr, "Erro: Falha na alocação de memória para valores constantes\n");
        free(tabela);
        exit(1);
    }
    
    return tabela;
}

// Libera a tabela de constantes
void liberarTabelaConstantes(TabelaConstantes *tabela) {
    if (tabela != NULL) {
        if (tabela->valores != NULL) {
            free(tabela->valores);
        }
        free(tabela);
    }
}

// Registra um valor constante na tabela
void registrarConstante(TabelaConstantes *tabela, const char *nome, const char *valor) {
    if (tabela == NULL || nome == NULL || valor == NULL) return;
    
    // Verifica se já existe
    for (int i = 0; i < tabela->tamanho; i++) {
        if (strcmp(tabela->valores[i].nome, nome) == 0) {
            // Atualiza o valor
            strcpy(tabela->valores[i].valor, valor);
            tabela->valores[i].ehConstante = 1;
            return;
        }
    }
    
    // Adiciona novo valor
    if (tabela->tamanho >= tabela->capacidade) {
        // Aumenta a capacidade
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

// Verifica se uma variável tem valor constante e retorna o valor
const char* obterValorConstante(TabelaConstantes *tabela, const char *nome) {
    if (tabela == NULL || nome == NULL) return NULL;
    
    for (int i = 0; i < tabela->tamanho; i++) {
        if (strcmp(tabela->valores[i].nome, nome) == 0 && tabela->valores[i].ehConstante) {
            return tabela->valores[i].valor;
        }
    }
    
    return NULL;
}

// Verifica se uma string é uma constante numérica ou booleana
int ehConstante(const char *nome) {
    if (nome == NULL || nome[0] == '\0') return 0;
    
    // Verifica se é true/false
    if (strcmp(nome, "true") == 0 || strcmp(nome, "false") == 0) {
        return 1;
    }
    
    // Verifica se é um número válido
    char *endptr;
    strtod(nome, &endptr);
    
    // Se endptr aponta para o final da string, então toda a string foi convertida
    return *endptr == '\0';
}

// Marca uma variável como usada na tabela
void marcarVariavelUsada(TabelaUsoVariaveis *tabela, const char *nome) {
    if (tabela == NULL || nome == NULL || nome[0] == '\0') return;
    
    // Ignora constantes
    if (ehConstante(nome)) return;
    
    // Procura a variável na tabela
    for (int i = 0; i < tabela->tamanho; i++) {
        if (strcmp(tabela->variaveis[i].nome, nome) == 0) {
            tabela->variaveis[i].usada = 1;
            return;
        }
    }
    
    // Se não encontrou, adiciona à tabela
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

// Verifica se uma variável está marcada como usada
int verificarVariavelUsada(TabelaUsoVariaveis *tabela, const char *nome) {
    if (tabela == NULL || nome == NULL || nome[0] == '\0') return 0;
    
    // Constantes são sempre consideradas "usadas"
    if (ehConstante(nome)) return 1;
    
    for (int i = 0; i < tabela->tamanho; i++) {
        if (strcmp(tabela->variaveis[i].nome, nome) == 0) {
            return tabela->variaveis[i].usada;
        }
    }
    
    return 0;  // Se não encontrou, considera não usada
}

// Analisa o uso de variáveis no código intermediário
void analisarUsoVariaveis(CodigoIntermediario *codigo, TabelaUsoVariaveis *tabela) {
    if (codigo == NULL || tabela == NULL) return;
    
    // Primeiro passo: registrar todas as variáveis definidas
    Instrucao *atual = codigo->inicio;
    while (atual != NULL) {
        // Adiciona variáveis definidas (resultado) à tabela com uso = 0
        if (atual->resultado[0] != '\0' && 
            atual->op != OP_LABEL && 
            atual->op != OP_JUMP && 
            atual->op != OP_CJUMP) {
            
            // Adiciona à tabela se ainda não existe
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
    
    // Segundo passo: marcar variáveis usadas
    atual = codigo->inicio;
    while (atual != NULL) {
        // Marca variáveis usadas em arg1 e arg2
        if (atual->arg1[0] != '\0') {
            marcarVariavelUsada(tabela, atual->arg1);
        }
        
        if (atual->arg2[0] != '\0') {
            marcarVariavelUsada(tabela, atual->arg2);
        }
        
        // Casos especiais
        switch (atual->op) {
            case OP_RETURN:
                if (atual->arg1[0] != '\0') {
                    marcarVariavelUsada(tabela, atual->arg1);
                }
                break;
                
            case OP_STORE:
                // O resultado de STORE é o nome da variável onde estamos armazenando
                marcarVariavelUsada(tabela, atual->resultado);
                break;
                
            case OP_PARAM:
                // Parâmetros são sempre considerados usados
                marcarVariavelUsada(tabela, atual->arg1);
                break;
                
            default:
                break;
        }
        
        atual = atual->prox;
    }
}

// Remove instruções cujo resultado não é usado (código morto)
void removerCodigoMorto(CodigoIntermediario *codigo) {
    if (codigo == NULL || codigo->inicio == NULL) return;

    int removidas = 0;

    // Cria tabela para rastrear uso de variáveis
    TabelaUsoVariaveis *tabela = criarTabelaUsoVariaveis();

    // Primeiro passo: marcar todas as variáveis usadas
    Instrucao *atual = codigo->inicio;
    while (atual != NULL) {
        // Marca uso em arg1 e arg2
        if (atual->arg1[0] != '\0') {
            marcarVariavelUsada(tabela, atual->arg1);
        }
        if (atual->arg2[0] != '\0') {
            marcarVariavelUsada(tabela, atual->arg2);
        }

        // Marca uso em condições de salto e atribuições a variáveis não temporárias
        if (atual->op == OP_CJUMP || atual->op == OP_JUMP) {
            marcarVariavelUsada(tabela, atual->resultado);
        }
        
        // Variáveis não temporárias são sempre consideradas usadas
        if (!ehTemporario(atual->resultado)) {
            marcarVariavelUsada(tabela, atual->resultado);
        }

        atual = atual->prox;
    }

    // Segundo passo: remover instruções que definem temporários não usados
    Instrucao *anterior = NULL;
    atual = codigo->inicio;

    while (atual != NULL) {
        int remover = 0;

        // Verifica se a instrução define um temporário não usado
        if (ehTemporario(atual->resultado) && !verificarVariavelUsada(tabela, atual->resultado)) {
            // Não remover chamadas de função, mesmo que o resultado não seja usado
            if (atual->op != OP_CALL) {
                remover = 1;
                removidas++;
            }
        }

        if (remover) {
            // Remove a instrução da lista
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

// Simplifica expressões com constantes
void simplificarExpressoes(CodigoIntermediario *codigo) {
    if (codigo == NULL) return;

    int simplificadas = 0;
    
    // Criar tabela para rastrear valores constantes
    TabelaConstantes *tabela = criarTabelaConstantes();
    
    // Primeiro passo: identificar atribuições diretas de constantes
    Instrucao *atual = codigo->inicio;
    while (atual != NULL) {
        // Caso 1: Atribuição direta de constante (t1 = 5)
        if (atual->op == OP_ASSIGN && ehConstante(atual->arg1)) {
            registrarConstante(tabela, atual->resultado, atual->arg1);
        }
        atual = atual->prox;
    }
    
    // Segundo passo: simplificar expressões usando constantes conhecidas
    atual = codigo->inicio;
    while (atual != NULL) {
        // Pular atribuições simples
        if (atual->op == OP_ASSIGN) {
            atual = atual->prox;
            continue;
        }
        
        // Verificar se os operandos são constantes ou têm valores constantes conhecidos
        const char *val1 = NULL;
        const char *val2 = NULL;
        
        // Verificar arg1
        if (ehConstante(atual->arg1)) {
            val1 = atual->arg1;
        } else {
            val1 = obterValorConstante(tabela, atual->arg1);
        }
        
        // Verificar arg2
        if (ehConstante(atual->arg2)) {
            val2 = atual->arg2;
        } else {
            val2 = obterValorConstante(tabela, atual->arg2);
        }
        
        // Se ambos os operandos têm valores constantes conhecidos
        if (val1 != NULL && val2 != NULL) {
            double num1 = atof(val1);
            double num2 = atof(val2);
            double resultado = 0;
            int calculado = 1; // Flag para indicar se o cálculo foi feito

            switch (atual->op) {
                case OP_ADD: resultado = num1 + num2; break;
                case OP_SUB: resultado = num1 - num2; break;
                case OP_MUL: resultado = num1 * num2; break;
                case OP_DIV:
                    if (num2 != 0) {
                        resultado = num1 / num2;
                    } else {
                        // Divisão por zero, não otimizar
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
                    calculado = 0; // Operação não suportada para simplificação
                    break;
            }

            if (calculado) {
                simplificadas++;
                
                // Transforma a instrução em uma atribuição
                atual->op = OP_ASSIGN;
                
                // Converte o resultado para string e armazena em arg1
                char valor[50];
                sprintf(valor, "%g", resultado);
                strcpy(atual->arg1, valor);
                
                // Limpa arg2
                atual->arg2[0] = '\0';
                
                // Registrar o resultado como constante para uso futuro
                registrarConstante(tabela, atual->resultado, valor);
            }
        }
        
        atual = atual->prox;
    }
    
    liberarTabelaConstantes(tabela);
}

// Função principal que chama as otimizações
CodigoIntermediario* otimizarCodigoIntermediario(CodigoIntermediario *codigo) {
    if (codigo == NULL) return NULL;

    printf("\n===== APLICANDO OTIMIZAÇÕES =====\n");

    // Executa as otimizações em um loop para garantir a aplicação completa
    // (e.g., simplificação pode gerar código morto)
    int alterado = 1;
    while (alterado) {
        alterado = 0;

        // Guardar o estado do código antes das otimizações
        // (uma forma simples é contar as instruções)
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

