#include "ast.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tabela.h"

// Variável para rastrear se estamos dentro de um branch de if-else
static int dentroDeBranchIfElse = 0;

NoAST *criarNoNum(int valor) {
  NoAST *novo = malloc(sizeof(NoAST));
  novo->tipo = NO_NUMERO;
  novo->operador = ' ';
  novo->valor = valor;
  novo->valorFloat = 0.0;      // Inicializar campo float
  novo->esquerda = novo->direita = NULL;
  novo->condicao = novo->corpo = novo->passo = NULL;
  novo->proximoIrmao = NULL;
  return novo;
}

NoAST *criarNoFloat(float valor) {
  NoAST *novo = malloc(sizeof(NoAST));
  novo->tipo = NO_FLOAT;
  novo->operador = 'f';
  novo->valor = 0;
  novo->valorFloat = valor;    // Armazenar valor float
  novo->esquerda = novo->direita = NULL;
  novo->condicao = novo->corpo = novo->passo = NULL;
  novo->proximoIrmao = NULL;
  return novo;
}

NoAST *criarNoId(char *nome) {
  NoAST *novo = malloc(sizeof(NoAST));
  novo->tipo = NO_ID;
  novo->operador = 'i';
  strcpy(novo->nome, nome);
  novo->valor = 0;
  novo->valorFloat = 0.0;      // Inicializar campo float
  novo->esquerda = novo->direita = NULL;
  novo->condicao = novo->corpo = novo->passo = NULL;
  novo->proximoIrmao = NULL;
  return novo;
}

NoAST *criarNoOp(char operador, NoAST *esq, NoAST *dir) {
  NoAST *novo = malloc(sizeof(NoAST));
  novo->tipo = NO_OPERADOR;
  novo->operador = operador;
  novo->esquerda = esq;
  novo->direita = dir;
  novo->condicao = novo->corpo = novo->passo = NULL;
  novo->proximoIrmao = NULL;
  return novo;
}

NoAST *criarNoAtribuicao(NoAST *id, NoAST *expressao) {
  NoAST *novo = malloc(sizeof(NoAST));
  novo->tipo = NO_ATRIBUICAO;
  novo->operador = '=';
  novo->esquerda = id;
  novo->direita = expressao;
  novo->condicao = novo->corpo = novo->passo = NULL;
  novo->proximoIrmao = NULL;
  return novo;
}

NoAST *criarNoIf(NoAST *condicao, NoAST *entao, NoAST *senao) {
  NoAST *novo = malloc(sizeof(NoAST));
  novo->tipo = NO_IF;
  novo->condicao = condicao;
  novo->esquerda = entao;
  novo->direita = senao; 
  novo->corpo = novo->passo = NULL;
  novo->proximoIrmao = NULL;
  return novo;
}

NoAST *criarNoWhile(NoAST *condicao, NoAST *corpo) {
  NoAST *novo = malloc(sizeof(NoAST));
  novo->tipo = NO_WHILE;
  novo->condicao = condicao;
  novo->corpo = corpo;
  novo->esquerda = novo->direita = novo->passo = NULL;
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
  novo->passo = passo;        // Passo da iteração (para range(inicio, fim, passo))
  novo->corpo = corpo;        // Corpo do loop
  novo->proximoIrmao = NULL;  // Inicializar como NULL
  return novo;
}

NoAST *criarNoDeclaracao(char *tipo, char *nome, NoAST *inicializacao) {
  NoAST *novo = malloc(sizeof(NoAST));
  novo->tipo = NO_DECLARACAO;
  strcpy(novo->nome, nome);
  novo->operador = tipo[0];      
  novo->direita = inicializacao;
  novo->esquerda = novo->condicao = novo->corpo = novo->passo = NULL;
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
  novo->direita = novo->condicao = novo->passo = NULL;
  novo->proximoIrmao = NULL;
  return novo;
}

NoAST *criarNoChamada(char *nome, NoAST *argumentos) {
  NoAST *novo = malloc(sizeof(NoAST));
  novo->tipo = NO_CHAMADA;
  strcpy(novo->nome, nome);
  novo->esquerda = argumentos;
  novo->direita = novo->condicao = novo->corpo = novo->passo = NULL;
  novo->proximoIrmao = NULL;
  return novo;
}

NoAST *criarNoBloco(NoAST *declaracoes) {
  NoAST *novo = malloc(sizeof(NoAST));
  novo->tipo = NO_BLOCO;
  novo->esquerda = declaracoes;
  novo->direita = novo->condicao = novo->corpo = novo->passo = NULL;
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
    case NO_FLOAT:           // Adicionar caso para float
      printf("%.2f", raiz->valorFloat);
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
  case NO_FLOAT:           // Adicionar caso para float
    printf("Número Float: %.2f\n", raiz->valorFloat);
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
    
    gerarCodigoC(arg, saida);

    if (arg->proximoIrmao) {
        fprintf(saida, ", ");
        gerarArgumentos(arg->proximoIrmao, saida);
    }
}

char* inferirTipoExpressao(NoAST *expr) {
    if (!expr) return "int";
    
    switch(expr->tipo) {
        case NO_NUMERO:
            return "int";
        case NO_FLOAT:           // Adicionar caso para float
            return "float";
        case NO_ID:
            // Buscar o tipo da variável na tabela de símbolos
            {
                Simbolo *s = buscarSimbolo(expr->nome);
                if (s) return s->tipo;
                return "int"; // Tipo padrão se não encontrado
            }
        case NO_OPERADOR:
            switch(expr->operador) {
                case 'T': case 'F': // TRUE/FALSE
                    return "bool";
                case 'f': // Float literal (operador antigo, manter compatibilidade)
                    return "float";
                case '+': case '-': case '*': case '/':
                    // Para operações aritméticas, verificar os operandos
                    {
                        char *tipo1 = inferirTipoExpressao(expr->esquerda);
                        char *tipo2 = inferirTipoExpressao(expr->direita);
                        if (strcmp(tipo1, "float") == 0 || strcmp(tipo2, "float") == 0)
                            return "float";
                        return "int";
                    }
                case '<': case '>': case 'l': case 'g': case '=': case '!':
                case '&': case '|': // Operações de comparação e lógicas
                    return "bool";
                default:
                    return "int";
            }
        default:
            return "int";
    }
}

// Estrutura para rastrear variáveis já declaradas
typedef struct VarDeclarada {
    char nome[100];
    struct VarDeclarada *prox;
} VarDeclarada;

static VarDeclarada *variaveisDeclaradas = NULL;

int variavelJaDeclarada(char *nome) {
    VarDeclarada *atual = variaveisDeclaradas;
    while (atual) {
        if (strcmp(atual->nome, nome) == 0) {
            return 1;
        }
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
    
    // Marcar este parâmetro como já declarado
    marcarVariavelDeclarada(param->nome);
    
    // Marcar os próximos parâmetros
    if (param->proximoIrmao) {
        marcarParametrosDeclarados(param->proximoIrmao);
    }
}

void gerarCodigoC(NoAST *raiz, FILE *saida) {
    if (!raiz) return;
    
    switch(raiz->tipo) {
        case NO_NUMERO:
            fprintf(saida, "%d", raiz->valor);
            break;
            
        case NO_FLOAT:           // Adicionar caso para float
            fprintf(saida, "%.6f", raiz->valorFloat);
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
            } else if (raiz->operador == 'T') {  // TRUE
                fprintf(saida, "true");
            } else if (raiz->operador == 'F') {  // FALSE
                fprintf(saida, "false");
            } else if (raiz->operador == 'f') {  // Float literal (compatibilidade)
                fprintf(saida, "0.0");
            } else if (raiz->operador == '~') {  // NOT (unário)
                fprintf(saida, "!(");
                gerarCodigoC(raiz->esquerda, saida);
                fprintf(saida, ")");
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
                    case 'l': fprintf(saida, " <= "); break;  // l para '≤'
                    case 'g': fprintf(saida, " >= "); break;  // g para '≥'
                    case '=': fprintf(saida, " == "); break;  // = para '=='
                    case '!': fprintf(saida, " != "); break;  // ! para '!='
                    case '&': fprintf(saida, " && "); break; // AND
                    case '|': fprintf(saida, " || "); break; // OR
                    default:  fprintf(saida, " %c ", raiz->operador);
                }
                
                gerarCodigoC(raiz->direita, saida);
                fprintf(saida, ")");
            }
            break;        case NO_ATRIBUICAO:
            {
                char *nome = raiz->esquerda->nome;
                
                // Verificar se a variável já foi declarada
                // Em branches de if-else, permitir redeclaração independente
                if (!variavelJaDeclarada(nome) || dentroDeBranchIfElse) {
                    // Inferir o tipo da expressão
                    char *tipo = inferirTipoExpressao(raiz->direita);
                    
                    // Declarar a variável com o tipo inferido apenas se estivermos em branch if-else
                    // ou se a variável realmente não foi declarada
                    if (!variavelJaDeclarada(nome)) {
                        fprintf(saida, "%s ", tipo);
                        
                        // Marcar como declarada
                        marcarVariavelDeclarada(nome);
                        
                        // Inserir na tabela de símbolos se ainda não estiver lá
                        Simbolo *s = buscarSimbolo(nome);
                        if (!s) {
                            inserirSimbolo(nome, tipo);
                        }
                    } else if (dentroDeBranchIfElse) {
                        // Se estamos em branch if-else e a variável já foi declarada,
                        // ainda assim declarar localmente no branch
                        fprintf(saida, "%s ", tipo);
                    }
                }
                
                // Gerar a atribuição
                gerarCodigoC(raiz->esquerda, saida);
                fprintf(saida, " = ");
                gerarCodigoC(raiz->direita, saida);
                fprintf(saida, ";\n");
            }
            break;        case NO_IF:
            fprintf(saida, "if (");
            gerarCodigoC(raiz->condicao, saida);
            fprintf(saida, ") {\n");
            
            // Salvar o estado atual das variáveis declaradas
            VarDeclarada *estadoOriginal = variaveisDeclaradas;
            
            // Marcar que estamos dentro de um branch if-else
            dentroDeBranchIfElse = 1;
            
            gerarCodigoC(raiz->esquerda, saida);  // Bloco "então"
            fprintf(saida, "}\n");
            
            if (raiz->direita) {  // Bloco "senão"
                // Restaurar o estado original para o bloco else
                variaveisDeclaradas = estadoOriginal;
                
                // Manter o contexto de branch if-else
                dentroDeBranchIfElse = 1;
                
                fprintf(saida, "else {\n");
                gerarCodigoC(raiz->direita, saida);
                fprintf(saida, "}\n");
            }
            
            // Sair do contexto de branch if-else
            dentroDeBranchIfElse = 0;
            break;
            
        case NO_WHILE:
            fprintf(saida, "while (");
            gerarCodigoC(raiz->condicao, saida);
            fprintf(saida, ") {\n");
            gerarCodigoC(raiz->corpo, saida);
            fprintf(saida, "}\n");
            break;
              case NO_FOR:
            {
                // Para loops for, gerar um for em C
                fprintf(saida, "for (");
                
                // Declarar/inicializar a variável de iteração se necessário
                char *nomeVar = raiz->esquerda->nome;
                if (!variavelJaDeclarada(nomeVar)) {
                    fprintf(saida, "int %s = ", nomeVar);
                    marcarVariavelDeclarada(nomeVar);
                    inserirSimbolo(nomeVar, "int");
                } else {
                    fprintf(saida, "%s = ", nomeVar);
                }
                gerarCodigoC(raiz->direita, saida); // valor inicial
                
                fprintf(saida, "; %s < ", nomeVar);
                gerarCodigoC(raiz->condicao, saida); // valor final
                
                fprintf(saida, "; %s += ", nomeVar);
                if (raiz->passo) {
                    gerarCodigoC(raiz->passo, saida); // passo
                } else {
                    fprintf(saida, "1"); // passo padrão
                }
                
                fprintf(saida, ") {\n");
                gerarCodigoC(raiz->corpo, saida);
                fprintf(saida, "}\n");
            }
            break;
            
        case NO_DECLARACAO:
            // Declaração explícita com tipo
            fprintf(saida, "%s %s", obterTipoC(raiz->operador), raiz->nome);
            marcarVariavelDeclarada(raiz->nome);
            
            if (raiz->direita) {  // Se tem inicialização
                fprintf(saida, " = ");
                gerarCodigoC(raiz->direita, saida);
            }
            
            fprintf(saida, ";\n");
            break;
              case NO_FUNCAO:
            // Inserir à tabela de símbolos
            inserirSimbolo(raiz->nome, "function");
            
            // Adicionar cabeçalhos necessários no início da função principal
            if (strcmp(raiz->nome, "main") == 0) {
                fprintf(saida, "#include <stdio.h>\n");
                fprintf(saida, "#include <stdbool.h>\n\n");
            }
            
            // Gerar cabeçalho da função
            fprintf(saida, "%s %s(", obterTipoC(raiz->operador), raiz->nome);
            
            // Gerar parâmetros
            gerarParametros(raiz->esquerda, saida);
            
            // Gerar corpo da função
            fprintf(saida, ") {\n");
            
            // Limpar lista de variáveis declaradas para o escopo da função
            liberarVariaveisDeclaradas();
            
            // Marcar os parâmetros como já declarados
            marcarParametrosDeclarados(raiz->esquerda);
            
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
        gerarCodigoC(raiz->proximoIrmao, saida);    }
}
