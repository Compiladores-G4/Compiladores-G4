#include "ast.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tabela.h"

static int dentroDeBranchIfElse = 0;
int nivelIndentacao = 0;

void imprimirIndentacao(FILE *saida) {
    for (int i = 0; i < nivelIndentacao; i++) {
        fprintf(saida, "    ");
    }
}

static NoAST *criarNoBase(TipoNo tipo, char operador) {
  NoAST *novo = malloc(sizeof(NoAST));
  novo->tipo = tipo;
  novo->operador = operador;
  novo->valor = 0;
  novo->valorFloat = 0.0;
  novo->esquerda = novo->direita = novo->condicao = novo->corpo = novo->passo = novo->proximoIrmao = NULL;
  return novo;
}

NoAST *criarNoNum(int valor) {
  NoAST *novo = criarNoBase(NO_NUMERO, ' ');
  novo->valor = valor;
  return novo;
}

NoAST *criarNoFloat(float valor) {
  NoAST *novo = criarNoBase(NO_FLOAT, 'f');
  novo->valorFloat = valor;
  return novo;
}

NoAST *criarNoId(char *nome) {
  NoAST *novo = criarNoBase(NO_ID, 'i');
  strcpy(novo->nome, nome);
  return novo;
}

NoAST *criarNoOp(char operador, NoAST *esq, NoAST *dir) {
  NoAST *novo = criarNoBase(NO_OPERADOR, operador);
  novo->esquerda = esq;
  novo->direita = dir;
  return novo;
}

NoAST *criarNoAtribuicao(NoAST *id, NoAST *expressao) {
  NoAST *novo = criarNoBase(NO_ATRIBUICAO, '=');
  novo->esquerda = id;
  novo->direita = expressao;
  return novo;
}

NoAST *criarNoIf(NoAST *condicao, NoAST *entao, NoAST *senao) {
  NoAST *novo = criarNoBase(NO_IF, ' ');
  novo->condicao = condicao;
  novo->esquerda = entao;
  novo->direita = senao;
  return novo;
}

NoAST *criarNoWhile(NoAST *condicao, NoAST *corpo) {
  NoAST *novo = criarNoBase(NO_WHILE, ' ');
  novo->condicao = condicao;
  novo->corpo = corpo;
  return novo;
}

NoAST *criarNoFor(NoAST *variavel, NoAST *inicio, NoAST *fim, NoAST *passo, NoAST *corpo) {
  NoAST *novo = criarNoBase(NO_FOR, 'f');
  novo->esquerda = variavel;
  novo->direita = inicio;
  novo->condicao = fim;
  novo->passo = passo;
  novo->corpo = corpo;
  return novo;
}

NoAST *criarNoDeclaracao(char *tipo, char *nome, NoAST *inicializacao) {
  NoAST *novo = criarNoBase(NO_DECLARACAO, tipo[0]);
  strcpy(novo->nome, nome);
  novo->direita = inicializacao;
  return novo;
}

NoAST *criarNoFuncao(char *tipo, char *nome, NoAST *parametros, NoAST *corpo) {
  NoAST *novo = criarNoBase(NO_FUNCAO, tipo[0]);
  strcpy(novo->nome, nome);
  novo->esquerda = parametros;
  novo->corpo = corpo;
  return novo;
}

NoAST *criarNoChamada(char *nome, NoAST *argumentos) {
  NoAST *novo = criarNoBase(NO_CHAMADA, ' ');
  strcpy(novo->nome, nome);
  novo->esquerda = argumentos;
  return novo;
}

NoAST *criarNoBloco(NoAST *declaracoes) {
  NoAST *novo = criarNoBase(NO_BLOCO, ' ');
  novo->esquerda = declaracoes;
  novo->direita = NULL;
  novo->condicao = novo->corpo = NULL;
  novo->proximoIrmao = NULL;
  return novo;
}

NoAST *criarNoListaVazia(void) {
    NoAST *novo = malloc(sizeof(NoAST)); 
    novo->tipo = NO_LISTA;             
    novo->operador = '[';               
    novo->esquerda = novo->direita = NULL; 
    novo->condicao = novo->corpo = NULL; 
    novo->proximoIrmao = NULL;         
    return novo;                        
}

NoAST *criarNoLista(NoAST *primeiro, NoAST *resto) {
    NoAST *novo = malloc(sizeof(NoAST)); 
    novo->tipo = NO_LISTA;            
    novo->operador = ',';              
    novo->esquerda = primeiro;          
    novo->direita = resto;              
    novo->condicao = novo->corpo = NULL; 
    novo->proximoIrmao = NULL;         
    return novo;                        
}

NoAST *adicionarIrmao(NoAST *node, NoAST *irmao) {
  if (!node) return irmao;
  NoAST *temp = node;
  while (temp->proximoIrmao) temp = temp->proximoIrmao;
  temp->proximoIrmao = irmao;
  return node;
}

void imprimirAST(NoAST *raiz) {
  if (!raiz) return;
  switch (raiz->tipo) {
    case NO_NUMERO: printf("%d", raiz->valor); break;
    case NO_FLOAT: printf("%.2f", raiz->valorFloat); break;
    case NO_ID: printf("%s", raiz->nome); break;
    case NO_OPERADOR:
      printf("("); imprimirAST(raiz->esquerda); printf(" %c ", raiz->operador); imprimirAST(raiz->direita); printf(")");
      break;
    case NO_ATRIBUICAO:
      imprimirAST(raiz->esquerda); printf(" = "); imprimirAST(raiz->direita);
      break;
    case NO_IF:
      printf("if ("); imprimirAST(raiz->condicao); printf(") ");
      imprimirAST(raiz->esquerda);
      if (raiz->direita) { printf(" else "); imprimirAST(raiz->direita); }
      break;
    case NO_WHILE:
      printf("while ("); imprimirAST(raiz->condicao); printf(") "); imprimirAST(raiz->corpo);
      break;
    case NO_FOR:
      printf("for ("); imprimirAST(raiz->esquerda); printf(" = "); imprimirAST(raiz->direita); printf("; ");
      imprimirAST(raiz->condicao); printf("; ");
      raiz->proximoIrmao ? imprimirAST(raiz->proximoIrmao) : printf("++");
      printf(") "); imprimirAST(raiz->corpo);
      break;
    case NO_DECLARACAO:
      printf("%c %s", raiz->operador, raiz->nome);
      if (raiz->direita) { printf(" = "); imprimirAST(raiz->direita); }
      break;
    case NO_FUNCAO:
      printf("%c %s(", raiz->operador, raiz->nome); imprimirAST(raiz->esquerda); printf(") "); imprimirAST(raiz->corpo);
      break;
    case NO_CHAMADA:
      printf("%s(", raiz->nome); imprimirAST(raiz->esquerda); printf(")");
      break;
    case NO_BLOCO:
      printf("{ "); imprimirAST(raiz->esquerda); printf(" }");
      break;
    case NO_LISTA: 
    if (raiz->operador == '[') { 
        printf("[]");            
    } else {
        printf("[");                
        imprimirAST(raiz->esquerda); 
        if (raiz->direita) {        
            printf(", ");           
            imprimirAST(raiz->direita); 
        }
        printf("]");                 
    }
    break;
  }
  if (raiz->proximoIrmao) { printf(", "); imprimirAST(raiz->proximoIrmao); }
}

void imprimirASTDetalhada(NoAST *raiz, int nivel) {
  if (!raiz) return;
  for (int i = 0; i < nivel; i++) printf("  ");
  switch (raiz->tipo) {
    case NO_NUMERO: printf("Número: %d\n", raiz->valor); break;
    case NO_FLOAT: printf("Número Float: %.2f\n", raiz->valorFloat); break;
    case NO_ID: printf("Identificador: %s\n", raiz->nome); break;
    case NO_OPERADOR: printf("Operador: %c\n", raiz->operador); break;
    case NO_ATRIBUICAO: printf("Atribuição:\n"); break;
    case NO_IF: printf("If:\n"); break;
    case NO_WHILE: printf("While:\n"); break;
    case NO_FOR: printf("For:\n"); break;
    case NO_DECLARACAO: printf("Declaração: %c %s\n", raiz->operador, raiz->nome); break;
    case NO_FUNCAO: printf("Função: %c %s\n", raiz->operador, raiz->nome); break;
    case NO_CHAMADA: printf("Chamada: %s\n", raiz->nome); break;
    case NO_BLOCO: printf("Bloco:\n"); break;
    case NO_LISTA: 
      if (raiz->operador == '[') {
        printf("Lista Vazia\n");
      } else {
        printf("Lista:\n");
      }
      break;

  }
  if (raiz->condicao) {
    for (int i = 0; i < nivel + 1; i++) printf("  ");
    printf("Condição:\n"); imprimirASTDetalhada(raiz->condicao, nivel + 2);
  }
  if (raiz->esquerda) {
    for (int i = 0; i < nivel + 1; i++) printf("  ");
    printf("%s:\n", raiz->tipo == NO_IF ? "Bloco Then" : raiz->tipo == NO_FOR ? "Variável" : "Esquerda");
    imprimirASTDetalhada(raiz->esquerda, nivel + 2);
  }
  if (raiz->direita) {
    for (int i = 0; i < nivel + 1; i++) printf("  ");
    printf("%s:\n", raiz->tipo == NO_IF ? "Bloco Else" : raiz->tipo == NO_FOR ? "Início" : "Direita");
    imprimirASTDetalhada(raiz->direita, nivel + 2);
  }
  if (raiz->corpo) {
    for (int i = 0; i < nivel + 1; i++) printf("  ");
    printf("Corpo:\n"); imprimirASTDetalhada(raiz->corpo, nivel + 2);
  }
  if (raiz->proximoIrmao) imprimirASTDetalhada(raiz->proximoIrmao, nivel);
}

void liberarAST(NoAST *raiz) {
  if (!raiz) return;
  liberarAST(raiz->esquerda);
  liberarAST(raiz->direita);
  liberarAST(raiz->condicao);
  liberarAST(raiz->corpo);
  liberarAST(raiz->proximoIrmao);
  free(raiz);
}

char* obterTipoC(char operador) {
  switch (operador) {
    case 'i': return "int";
    case 'f': return "float";
    case 'b': return "bool";
    case 'v': return "void";
    default: return "int";
  }
}

void gerarParametros(NoAST *param, FILE *saida) {
  if (!param) return;
  fprintf(saida, "%s %s", obterTipoC(param->operador), param->nome);
  if (param->proximoIrmao) { fprintf(saida, ", "); gerarParametros(param->proximoIrmao, saida); }
}

void gerarArgumentos(NoAST *arg, FILE *saida) {
  if (!arg) return;
  gerarCodigoC(arg, saida);
  if (arg->proximoIrmao) { fprintf(saida, ", "); gerarArgumentos(arg->proximoIrmao, saida); }
}

char* inferirTipoExpressao(NoAST *expr) {
  if (!expr) return "int";
  switch (expr->tipo) {
    case NO_NUMERO: return "int";
    case NO_FLOAT: return "float";
    case NO_ID: {
      Simbolo *s = buscarSimbolo(expr->nome);
      return s ? s->tipo : "int";
    }
    case NO_LISTA: return "int[]";  // Arrays de inteiros
    case NO_OPERADOR:
      switch (expr->operador) {
        case 'T': case 'F': return "bool";
        case 'f': return "float";
        case '+': case '-': case '*': case '/': {
          char *tipo1 = inferirTipoExpressao(expr->esquerda);
          char *tipo2 = inferirTipoExpressao(expr->direita);
          return (strcmp(tipo1, "float") == 0 || strcmp(tipo2, "float") == 0) ? "float" : "int";
        }
        case '<': case '>': case 'l': case 'g': case '=': case '!': case '&': case '|': return "bool";
        default: return "int";
      }
    default: return "int";
  }
}

typedef struct VarDeclarada {
  char nome[100];
  struct VarDeclarada *prox;
} VarDeclarada;

static VarDeclarada *variaveisDeclaradas = NULL;

int variavelJaDeclarada(char *nome) {
  VarDeclarada *atual = variaveisDeclaradas;
  while (atual) {
    if (strcmp(atual->nome, nome) == 0) return 1;
    atual = atual->prox;
  }
  return 0;
}

void marcarVariavelDeclarada(char *nome) {
  if (variavelJaDeclarada(nome)) return;
  VarDeclarada *nova = malloc(sizeof(VarDeclarada));
  strcpy(nova->nome, nome);
  nova->prox = variaveisDeclaradas;
  variaveisDeclaradas = nova;
}

void liberarVariaveisDeclaradas() {
  VarDeclarada *atual = variaveisDeclaradas;
  while (atual) {
    VarDeclarada *temp = atual;
    atual = atual->prox;
    free(temp);
  }
  variaveisDeclaradas = NULL;
}

void marcarParametrosDeclarados(NoAST *param) {
  if (!param) return;
  marcarVariavelDeclarada(param->nome);
  marcarParametrosDeclarados(param->proximoIrmao);
}

void gerarElementosLista(NoAST *lista, FILE *saida) {
    if (!lista) return;
    
    if (lista->tipo == NO_LISTA) {
        if (lista->operador == '[') {
            return;
        } else {
            gerarCodigoC(lista->esquerda, saida);
            if (lista->direita) {
                fprintf(saida, ", ");
                gerarElementosLista(lista->direita, saida);
            }
        }
    } else {
        gerarCodigoC(lista, saida);
    }
}

void gerarCodigoC(NoAST *raiz, FILE *saida) {
    if (!raiz) return;

    switch (raiz->tipo) {
        case NO_NUMERO: fprintf(saida, "%d", raiz->valor); break;
        case NO_FLOAT: fprintf(saida, "%.6f", raiz->valorFloat); break;
        case NO_ID: fprintf(saida, "%s", raiz->nome); break;
        case NO_OPERADOR: {
            char op = raiz->operador;
            if (op == 'r') {
                fprintf(saida, "return ");
                if (raiz->esquerda) gerarCodigoC(raiz->esquerda, saida);
                fprintf(saida, ";\n");
            } else if (op == 'T') fprintf(saida, "true");
            else if (op == 'F') fprintf(saida, "false");
            else if (op == 'f') fprintf(saida, "0.0");
            else if (op == '~') {
                fprintf(saida, "!("); gerarCodigoC(raiz->esquerda, saida); fprintf(saida, ")");
            } else {
                fprintf(saida, "("); gerarCodigoC(raiz->esquerda, saida);
                const char *ops[] = {" + ", " - ", " * ", " / ", " < ", " > ", " <= ", " >= ", " == ", " != ", " && ", " || "};
                char op_map[] = {'+', '-', '*', '/', '<', '>', 'l', 'g', '=', '!', '&', '|'};
                int i;
                for (i = 0; i < 12 && op_map[i] != op; i++);
                fprintf(saida, i < 12 ? ops[i] : " %c ", op);
                gerarCodigoC(raiz->direita, saida); fprintf(saida, ")");
            }
        } break;        case NO_ATRIBUICAO: {
            char *nome = raiz->esquerda->nome;
            char *tipo = inferirTipoExpressao(raiz->direita);
            if (raiz->direita->tipo == NO_LISTA) {
                if (!variavelJaDeclarada(nome)) {
                    imprimirIndentacao(saida);
                    if (raiz->direita->operador == '[') {
                        fprintf(saida, "int *%s = NULL; // Lista vazia\n", nome);
                    } else {
                        fprintf(saida, "int %s[] = {", nome);
                        gerarElementosLista(raiz->direita, saida);
                        fprintf(saida, "};\n");
                    }
                    marcarVariavelDeclarada(nome);
                    if (!buscarSimbolo(nome)) inserirSimbolo(nome, "int[]");
                } else {
                    imprimirIndentacao(saida);
                    fprintf(saida, "// Reatribuição de array não suportada diretamente\n");
                }
            } else {
                imprimirIndentacao(saida);
                if (!variavelJaDeclarada(nome) || dentroDeBranchIfElse) {
                    if (!variavelJaDeclarada(nome) || dentroDeBranchIfElse)
                        fprintf(saida, "%s ", tipo);
                    if (!variavelJaDeclarada(nome)) {
                        marcarVariavelDeclarada(nome);
                        if (!buscarSimbolo(nome)) inserirSimbolo(nome, tipo);
                    }
                }
                gerarCodigoC(raiz->esquerda, saida); fprintf(saida, " = ");
                gerarCodigoC(raiz->direita, saida); fprintf(saida, ";\n");
            }
        } break;case NO_IF: {
            imprimirIndentacao(saida);
            fprintf(saida, "if ("); gerarCodigoC(raiz->condicao, saida); fprintf(saida, ") {\n");
            VarDeclarada *estado = variaveisDeclaradas;
            dentroDeBranchIfElse = 1;
            nivelIndentacao++;
            gerarCodigoC(raiz->esquerda, saida);
            nivelIndentacao--;
            imprimirIndentacao(saida);
            fprintf(saida, "}\n");
            if (raiz->direita) {
                variaveisDeclaradas = estado;
                dentroDeBranchIfElse = 1;
                imprimirIndentacao(saida);
                fprintf(saida, "else {\n");
                nivelIndentacao++;
                gerarCodigoC(raiz->direita, saida);
                nivelIndentacao--;
                imprimirIndentacao(saida);
                fprintf(saida, "}\n");
            }
            dentroDeBranchIfElse = 0;
        } break;
        case NO_WHILE:
            fprintf(saida, "while ("); gerarCodigoC(raiz->condicao, saida);
            fprintf(saida, ") {\n"); gerarCodigoC(raiz->corpo, saida); fprintf(saida, "}\n"); break;
        case NO_FOR: {
            char *nome = raiz->esquerda->nome;
            fprintf(saida, "for (");
            if (!variavelJaDeclarada(nome)) {
                fprintf(saida, "int %s = ", nome); marcarVariavelDeclarada(nome); inserirSimbolo(nome, "int");
            } else fprintf(saida, "%s = ", nome);
            gerarCodigoC(raiz->direita, saida); fprintf(saida, "; %s < ", nome);
            gerarCodigoC(raiz->condicao, saida); fprintf(saida, "; %s += ", nome);
            raiz->passo ? gerarCodigoC(raiz->passo, saida) : fprintf(saida, "1");
            fprintf(saida, ") {\n"); gerarCodigoC(raiz->corpo, saida); fprintf(saida, "}\n");
        } break;
        case NO_DECLARACAO:
            fprintf(saida, "%s %s", obterTipoC(raiz->operador), raiz->nome);
            marcarVariavelDeclarada(raiz->nome);
            if (raiz->direita) { fprintf(saida, " = "); gerarCodigoC(raiz->direita, saida); }
            fprintf(saida, ";\n"); break;
        case NO_FUNCAO: {
            inserirSimbolo(raiz->nome, "function");
            fprintf(saida, "%s %s(", obterTipoC(raiz->operador), raiz->nome);
            gerarParametros(raiz->esquerda, saida); fprintf(saida, ") {\n");
            liberarVariaveisDeclaradas(); marcarParametrosDeclarados(raiz->esquerda);
            gerarCodigoC(raiz->corpo, saida); fprintf(saida, "}\n\n");        } break;        case NO_CHAMADA:
            if (strcmp(raiz->nome, "print") == 0) {
                imprimirIndentacao(saida);
                fprintf(saida, "printf(");
                if (raiz->esquerda) {
                    NoAST *arg = raiz->esquerda;
                    if (arg->tipo == NO_LISTA) {
                        arg = arg->esquerda;
                    }
                    
                    if (arg->tipo == NO_NUMERO) {
                        fprintf(saida, "\"%%d\\n\", ");
                        gerarCodigoC(arg, saida);
                    } else if (arg->tipo == NO_FLOAT) {
                        fprintf(saida, "\"%%f\\n\", ");
                        gerarCodigoC(arg, saida);
                    } else if (arg->tipo == NO_OPERADOR && (arg->operador == 'T' || arg->operador == 'F')) {
                        fprintf(saida, "\"%%s\\n\", ");
                        gerarCodigoC(arg, saida);
                        fprintf(saida, " ? \"True\" : \"False\"");
                    } else if (arg->tipo == NO_ID) {
                        char *tipo = inferirTipoExpressao(arg);
                        if (strcmp(tipo, "int") == 0) {
                            fprintf(saida, "\"%%d\\n\", ");
                        } else if (strcmp(tipo, "float") == 0) {
                            fprintf(saida, "\"%%f\\n\", ");
                        } else if (strcmp(tipo, "bool") == 0) {
                            fprintf(saida, "\"%%s\\n\", ");
                            gerarCodigoC(arg, saida);
                            fprintf(saida, " ? \"True\" : \"False\"");
                        } else {
                            fprintf(saida, "\"%%d\\n\", ");
                        }
                        if (strcmp(inferirTipoExpressao(arg), "bool") != 0) {
                            gerarCodigoC(arg, saida);
                        }
                    } else {
                        fprintf(saida, "\"%%d\\n\", ");
                        gerarCodigoC(arg, saida);
                    }
                } else {
                    fprintf(saida, "\"\\n\"");
                }
                fprintf(saida, ");\n");
            } else {
                fprintf(saida, "%s(", raiz->nome); 
                gerarArgumentos(raiz->esquerda, saida); 
                fprintf(saida, ")");
            }
            break;
        case NO_BLOCO: gerarCodigoC(raiz->esquerda, saida); break;
        case NO_LISTA: {
            fprintf(saida, "{");
            gerarElementosLista(raiz, saida);
            fprintf(saida, "}");
        } break;
    }
    if (raiz->proximoIrmao) gerarCodigoC(raiz->proximoIrmao, saida);
}