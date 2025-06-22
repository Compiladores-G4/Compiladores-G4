#include "ast.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tabela.h"

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
  novo->esquerda = entao;
  novo->direita = senao; 
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

NoAST *criarNoFor(NoAST *variavel, NoAST *inicio, NoAST *fim, NoAST *passo, NoAST *corpo) {
  NoAST *novo = malloc(sizeof(NoAST));
  novo->tipo = NO_FOR;
  novo->operador = 'f';
  novo->esquerda = variavel;  // Variável de iteração
  novo->direita = inicio;     // Valor inicial
  novo->condicao = fim;       // Valor final/condição limite
  novo->proximoIrmao = passo; // Passo da iteração (para range(inicio, fim, passo))
  novo->corpo = corpo;        // Corpo do loop
  return novo;
}

NoAST *criarNoDeclaracao(char *tipo, char *nome, NoAST *inicializacao) {
  NoAST *novo = malloc(sizeof(NoAST));
  novo->tipo = NO_DECLARACAO;
  strcpy(novo->nome, nome);
  novo->operador = tipo[0];      
  novo->direita = inicializacao;
  novo->esquerda = novo->condicao = novo->corpo = NULL;
  novo->proximoIrmao = NULL;
  return novo;
}

NoAST *criarNoFuncao(char *tipo, char *nome, NoAST *parametros, NoAST *corpo) {
  NoAST *novo = malloc(sizeof(NoAST));
  novo->tipo = NO_FUNCAO;
  strcpy(novo->nome, nome);
  novo->operador = tipo[0];
  novo->esquerda = parametros; 
  novo->corpo = corpo;  
  novo->direita = novo->condicao = NULL;
  novo->proximoIrmao = NULL;
  return novo;
}

NoAST *criarNoChamada(char *nome, NoAST *argumentos) {
  NoAST *novo = malloc(sizeof(NoAST));
  novo->tipo = NO_CHAMADA;
  strcpy(novo->nome, nome);
  novo->esquerda = argumentos;
  novo->direita = novo->condicao = novo->corpo = NULL;
  novo->proximoIrmao = NULL;
  return novo;
}

NoAST *criarNoBloco(NoAST *declaracoes) {
  NoAST *novo = malloc(sizeof(NoAST));
  novo->tipo = NO_BLOCO;
  novo->esquerda = declaracoes;
  novo->direita = novo->condicao = novo->corpo = NULL;
  novo->proximoIrmao = NULL;
  return novo;
}

NoAST *adicionarIrmao(NoAST *node, NoAST *irmao) {
  if (!node)
    return irmao;

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
    case NO_FOR:
      printf("for (");
      imprimirAST(raiz->esquerda);
      printf(" = ");
      imprimirAST(raiz->direita);
      printf("; ");
      imprimirAST(raiz->condicao);
      printf("; ");
      if (raiz->proximoIrmao) {
        imprimirAST(raiz->proximoIrmao);
      } else {
        printf("++"); // Incremento padrão se não especificado
      }
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

    if (raiz->proximoIrmao) {
      printf(", ");
      imprimirAST(raiz->proximoIrmao);
    }
  }
}

void imprimirASTDetalhada(NoAST *raiz, int nivel) {
  if (!raiz)
    return;

  for (int i = 0; i < nivel; i++) {
    printf("  ");
  }

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
    if (raiz->condicao) {
      for (int i = 0; i < nivel + 1; i++) {
        printf("  ");
      }
      printf("Condição:\n");
      imprimirASTDetalhada(raiz->condicao, nivel + 2);
    }
    if (raiz->esquerda) {
      for (int i = 0; i < nivel + 1; i++) {
        printf("  ");
      }
      printf("Bloco Then:\n");
      imprimirASTDetalhada(raiz->esquerda, nivel + 2);
    }
    if (raiz->direita) {
      for (int i = 0; i < nivel + 1; i++) {
        printf("  ");
      }
      printf("Bloco Else:\n");
      imprimirASTDetalhada(raiz->direita, nivel + 2);
    }
    break;
  case NO_WHILE:
    printf("While:\n");
    if (raiz->condicao) {
      for (int i = 0; i < nivel + 1; i++) {
        printf("  ");
      }
      printf("Condição:\n");
      imprimirASTDetalhada(raiz->condicao, nivel + 2);
    }
    if (raiz->corpo) {
      for (int i = 0; i < nivel + 1; i++) {
        printf("  ");
      }
      printf("Corpo:\n");
      imprimirASTDetalhada(raiz->corpo, nivel + 2);
    }
    break;
  case NO_FOR:
    printf("For:\n");
    if (raiz->esquerda) {
      for (int i = 0; i < nivel + 1; i++) {
        printf("  ");
      }
      printf("Variável:\n");
      imprimirASTDetalhada(raiz->esquerda, nivel + 2);
    }
    if (raiz->direita) {
      for (int i = 0; i < nivel + 1; i++) {
        printf("  ");
      }
      printf("Início:\n");
      imprimirASTDetalhada(raiz->direita, nivel + 2);
    }
    if (raiz->condicao) {
      for (int i = 0; i < nivel + 1; i++) {
        printf("  ");
      }
      printf("Fim:\n");
      imprimirASTDetalhada(raiz->condicao, nivel + 2);
    }
    if (raiz->proximoIrmao) {
      for (int i = 0; i < nivel + 1; i++) {
        printf("  ");
      }
      printf("Passo:\n");
      imprimirASTDetalhada(raiz->proximoIrmao, nivel + 2);
    }
    if (raiz->corpo) {
      for (int i = 0; i < nivel + 1; i++) {
        printf("  ");
      }
      printf("Corpo:\n");
      imprimirASTDetalhada(raiz->corpo, nivel + 2);
    }
    break;
  case NO_DECLARACAO:
    printf("Declaração: %c %s\n", raiz->operador, raiz->nome);
    break;
  case NO_FUNCAO:
    printf("Função: %c %s\n", raiz->operador, raiz->nome);
    if (raiz->esquerda) {
      for (int i = 0; i < nivel + 1; i++) {
        printf("  ");
      }
      printf("Parâmetros:\n");
      imprimirASTDetalhada(raiz->esquerda, nivel + 2);
    }
    if (raiz->corpo) {
      for (int i = 0; i < nivel + 1; i++) {
        printf("  ");
      }
      printf("Corpo:\n");
      imprimirASTDetalhada(raiz->corpo, nivel + 2);
    }
    break;
  case NO_CHAMADA:
    printf("Chamada: %s\n", raiz->nome);
    break;
  case NO_BLOCO:
    printf("Bloco:\n");
    break;
  }

  if (raiz->esquerda)
    imprimirASTDetalhada(raiz->esquerda, nivel + 1);
  if (raiz->direita)
    imprimirASTDetalhada(raiz->direita, nivel + 1);
  if (raiz->condicao)
    imprimirASTDetalhada(raiz->condicao, nivel + 1);
  if (raiz->corpo)
    imprimirASTDetalhada(raiz->corpo, nivel + 1);

  if (raiz->proximoIrmao)
    imprimirASTDetalhada(raiz->proximoIrmao, nivel);
}

void liberarAST(NoAST *raiz) {
  if (!raiz)
    return;

  liberarAST(raiz->esquerda);
  liberarAST(raiz->direita);
  liberarAST(raiz->condicao);
  liberarAST(raiz->corpo);
  liberarAST(raiz->proximoIrmao);

  free(raiz);
}

char* obterTipoC(char operador) {
    switch(operador) {
        case 'i': return "int";
        case 'f': return "float";
        case 'b': return "bool";
        case 'v': return "void";
        default: return "int";  // Tipo padrão
    }
}

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
    
    if (raiz->proximoIrmao) {
        gerarCodigoC(raiz->proximoIrmao, saida);
    }
}
