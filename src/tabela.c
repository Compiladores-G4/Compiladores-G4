#include "tabela.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

Simbolo *tabela = NULL;
Escopo *escopoAtual = NULL;
Escopo *escoposRaiz = NULL;

// Inicializa a estrutura de escopos com o escopo global
void inicializarEscopos() {
  if (escoposRaiz == NULL) {
    escoposRaiz = malloc(sizeof(Escopo));
    if (escoposRaiz == NULL) {
      fprintf(stderr, "Erro: Falha na alocação de memória para escopo global\n");
      exit(1);
    }
    strcpy(escoposRaiz->nome, "global");
    escoposRaiz->pai = NULL;
    escoposRaiz->prox = NULL;
    escoposRaiz->tipoRetorno = strdup("void"); // Escopo global não tem retorno
    escoposRaiz->parametros = NULL;
    escoposRaiz->numParametros = 0;
    escopoAtual = escoposRaiz;
  }
}

// Cria um novo escopo como filho do escopo atual
void criarEscopo(char *nome) {
  if (escopoAtual == NULL) {
    inicializarEscopos();
  }
  
  Escopo *novo = malloc(sizeof(Escopo));
  if (novo == NULL) {
    fprintf(stderr, "Erro: Falha na alocação de memória para escopo %s\n", nome);
    return;
  }
  
  strcpy(novo->nome, nome);
  novo->pai = escopoAtual;
  novo->prox = NULL;
  novo->tipoRetorno = strdup("void"); // Tipo de retorno padrão
  novo->parametros = NULL;
  novo->numParametros = 0;
  
  // Adicionar como próximo do último filho do escopo atual
  if (escopoAtual->prox == NULL) {
    escopoAtual->prox = novo;
  } else {
    Escopo *ultimo = escopoAtual->prox;
    while (ultimo->prox != NULL) {
      ultimo = ultimo->prox;
    }
    ultimo->prox = novo;
  }
  
  // Atualizar escopo atual
  escopoAtual = novo;
}

// Sai do escopo atual e volta para o escopo pai
void sairEscopo() {
  if (escopoAtual != NULL && escopoAtual->pai != NULL) {
    escopoAtual = escopoAtual->pai;
  }
}

// Retorna o escopo atual
Escopo *obterEscopoAtual() {
  if (escopoAtual == NULL) {
    inicializarEscopos();
  }
  return escopoAtual;
}

// Retorna o nome do escopo atual
char *obterNomeEscopoAtual() {
  if (escopoAtual == NULL) {
    inicializarEscopos();
  }
  return escopoAtual->nome;
}

// Insere um símbolo no escopo atual
void inserirSimbolo(char *nome, char *tipo) {
  char *escopoNome = obterNomeEscopoAtual();
  inserirSimboloComEscopo(nome, tipo, escopoNome);
}

// Insere um símbolo em um escopo específico
void inserirSimboloComEscopo(char *nome, char *tipo, char *escopo) {
  // Verifica se o símbolo já existe no mesmo escopo
  Simbolo *s = buscarSimboloNoEscopo(nome, escopo);
  if (s != NULL) {
    // O símbolo já existe, apenas atualiza o tipo se necessário
    if (strcmp(s->tipo, "desconhecido") == 0 && strcmp(tipo, "desconhecido") != 0) {
      strcpy(s->tipo, tipo);
    }
    return;
  }
  
  // O símbolo não existe, cria um novo
  Simbolo *novo = malloc(sizeof(Simbolo));
  if (novo == NULL) {
    fprintf(stderr, "Erro: Falha na alocação de memória para símbolo\n");
    return;
  }
  
  strcpy(novo->nome, nome);
  strcpy(novo->tipo, tipo);
  strcpy(novo->escopo, escopo);
  novo->valor = NULL;
  
  // Insere no início da lista
  novo->prox = tabela;
  tabela = novo;
}

// Busca um símbolo em um escopo específico
Simbolo *buscarSimboloNoEscopo(char *nome, char *escopo) {
  Simbolo *s = tabela;
  while (s != NULL) {
    if (strcmp(s->nome, nome) == 0 && strcmp(s->escopo, escopo) == 0) {
      return s;
    }
    s = s->prox;
  }
  return NULL;
}

// Busca um símbolo em toda a cadeia de escopos
Simbolo *buscarSimboloCadeiaEscopos(char *nome) {
  if (escopoAtual == NULL) {
    inicializarEscopos();
  }
  
  Escopo *e = escopoAtual;
  while (e != NULL) {
    Simbolo *s = buscarSimboloNoEscopo(nome, e->nome);
    if (s != NULL) {
      return s;
    }
    e = e->pai;
  }
  return NULL;
}

// Função simples de busca de símbolo (busca em cadeia)
Simbolo *buscarSimbolo(char *nome) {
  return buscarSimboloCadeiaEscopos(nome);
}

// Imprime a tabela de símbolos
void imprimirTabela() {
  printf("\n===== TABELA DE SÍMBOLOS =====\n");
  printf("Nome\t\tTipo\t\tEscopo\n");
  printf("-------------------------------\n");
  
  Simbolo *s = tabela;
  while (s != NULL) {
    printf("%s\t\t%s\t\t%s\n", s->nome, s->tipo, s->escopo);
    s = s->prox;
  }
  printf("===============================\n");
}

// Imprime a estrutura de escopos
void imprimirEscopos() {
  printf("\n===== ESTRUTURA DE ESCOPOS =====\n");
  
  void imprimirEscopoRecursivo(Escopo *e, int nivel) {
    if (e == NULL) return;
    
    for (int i = 0; i < nivel; i++) printf("  ");
    printf("%s\n", e->nome);
    
    Escopo *filho = e->prox;
    while (filho != NULL) {
      imprimirEscopoRecursivo(filho, nivel + 1);
      filho = filho->prox;
    }
  }
  
  imprimirEscopoRecursivo(escoposRaiz, 0);
  printf("=================================\n");
}

// Verifica se um identificador foi declarado
int verificarDeclaracao(char *nome) {
  Simbolo *s = buscarSimbolo(nome);
  if (s == NULL) {
    printf("Erro semântico: variável '%s' não declarada\n", nome);
    return 0;
  }
  return 1;
}

// Verifica se dois tipos são compatíveis
int tiposCompativeis(char *tipo1, char *tipo2) {
  // Tipos iguais são compatíveis
  if (strcmp(tipo1, tipo2) == 0) return 1;
  
  // Tipo desconhecido é compatível com qualquer coisa
  if (strcmp(tipo1, "desconhecido") == 0 || strcmp(tipo2, "desconhecido") == 0) return 1;
  
  // int é compatível com float (conversão implícita)
  if ((strcmp(tipo1, "int") == 0 && strcmp(tipo2, "float") == 0) ||
      (strcmp(tipo1, "float") == 0 && strcmp(tipo2, "int") == 0)) return 1;
  
  return 0;
}

// Verifica se uma atribuição é válida
int verificarAtribuicao(char *destino, char *origem) {
  Simbolo *s_dest = buscarSimbolo(destino);
  Simbolo *s_orig = buscarSimbolo(origem);
  
  if (s_dest == NULL || s_orig == NULL) return 0;
  
  if (!tiposCompativeis(s_dest->tipo, s_orig->tipo)) {
    printf("Erro semântico: atribuição incompatível de '%s' (%s) para '%s' (%s)\n", 
           origem, s_orig->tipo, destino, s_dest->tipo);
    return 0;
  }
  
  return 1;
}

// Determina o tipo resultante de uma operação
char* obterTipoResultante(char *tipo1, char *tipo2, char operador) {
  // Tipo desconhecido propaga
  if (strcmp(tipo1, "desconhecido") == 0 || strcmp(tipo2, "desconhecido") == 0) {
    return "desconhecido";
  }
  
  // Operadores de comparação sempre resultam em bool
  if (operador == '=' || operador == '!' || operador == '<' || operador == '>' || 
      operador == 'l' || operador == 'g') {
    return "bool";
  }
  
  // Se ambos são int, resultado é int
  if (strcmp(tipo1, "int") == 0 && strcmp(tipo2, "int") == 0) {
    return "int";
  }
  
  // Se um deles é float, resultado é float
  if (strcmp(tipo1, "float") == 0 || strcmp(tipo2, "float") == 0) {
    return "float";
  }
  
  // Para operações com bool, depende do operador
  if (strcmp(tipo1, "bool") == 0 && strcmp(tipo2, "bool") == 0) {
    if (operador == '+' || operador == '-' || operador == '*' || operador == '/') {
      return "int"; // Promoção de bool para int em operações aritméticas
    }
    return "bool";
  }
  
  return "desconhecido";
}

// Verifica se uma operação é válida entre dois identificadores
int verificarOperacao(char *nome1, char *nome2, char operador) {
  Simbolo *s1 = buscarSimbolo(nome1);
  Simbolo *s2 = buscarSimbolo(nome2);
  
  if (s1 == NULL || s2 == NULL) return 0;
  
  // Operadores lógicos só podem ser aplicados a bool
  if ((operador == '=' || operador == '!') && 
      (strcmp(s1->tipo, "bool") != 0 || strcmp(s2->tipo, "bool") != 0)) {
    if (strcmp(s1->tipo, "desconhecido") != 0 && strcmp(s2->tipo, "desconhecido") != 0) {
      printf("Erro semântico: operador '%c' requer operandos do tipo bool\n", operador);
      return 0;
    }
  }
  
  // Operações aritméticas só podem ser aplicadas a números
  if ((operador == '+' || operador == '-' || operador == '*' || operador == '/') && 
      !((strcmp(s1->tipo, "int") == 0 || strcmp(s1->tipo, "float") == 0 || strcmp(s1->tipo, "desconhecido") == 0) && 
        (strcmp(s2->tipo, "int") == 0 || strcmp(s2->tipo, "float") == 0 || strcmp(s2->tipo, "desconhecido") == 0))) {
    printf("Erro semântico: operador '%c' requer operandos numéricos\n", operador);
    return 0;
  }
  
  return 1;
}

// Define o tipo de retorno para o escopo atual
void definirTipoRetorno(char *tipo) {
  if (escopoAtual == NULL) {
    inicializarEscopos();
  }
  
  if (escopoAtual->tipoRetorno != NULL) {
    free(escopoAtual->tipoRetorno);
  }
  
  escopoAtual->tipoRetorno = strdup(tipo);
}

// Verifica se o tipo de retorno é compatível com o escopo atual
int verificarRetorno(char *tipo) {
  if (escopoAtual == NULL) {
    inicializarEscopos();
    return 1; // No escopo global, qualquer retorno é válido
  }
  
  // Se o tipo de retorno não foi definido, qualquer retorno é válido
  if (escopoAtual->tipoRetorno == NULL) {
    return 1;
  }
  
  // Verifica compatibilidade
  if (!tiposCompativeis(escopoAtual->tipoRetorno, tipo)) {
    printf("Erro semântico: retorno incompatível. Esperado '%s', encontrado '%s'\n", 
           escopoAtual->tipoRetorno, tipo);
    return 0;
  }
  
  return 1;
}

// Adiciona parâmetros a um escopo de função
void adicionarParametros(int num, ...) {
  if (escopoAtual == NULL) {
    inicializarEscopos();
    return;
  }
  
  // Libera parâmetros anteriores se existirem
  if (escopoAtual->parametros != NULL) {
    for (int i = 0; i < escopoAtual->numParametros; i++) {
      free(escopoAtual->parametros[i].nome);
      free(escopoAtual->parametros[i].tipo);
    }
    free(escopoAtual->parametros);
  }
  
  // Aloca novos parâmetros
  escopoAtual->numParametros = num;
  if (num > 0) {
    escopoAtual->parametros = malloc(num * sizeof(Parametro));
    if (escopoAtual->parametros == NULL) {
      fprintf(stderr, "Erro: Falha na alocação de memória para parâmetros\n");
      return;
    }
    
    va_list args;
    va_start(args, num);
    
    for (int i = 0; i < num; i++) {
      char *nome = va_arg(args, char*);
      char *tipo = va_arg(args, char*);
      
      escopoAtual->parametros[i].nome = strdup(nome);
      escopoAtual->parametros[i].tipo = strdup(tipo);
      
      // Também insere o parâmetro na tabela de símbolos
      inserirSimbolo(nome, tipo);
    }
    
    va_end(args);
  } else {
    escopoAtual->parametros = NULL;
  }
}

// Verifica se uma chamada de função tem o número correto de argumentos
int verificarArgumentos(char *nomeFuncao, int numArgs) {
  // Busca o escopo da função
  Escopo *e = escoposRaiz;
  while (e != NULL) {
    if (strcmp(e->nome, nomeFuncao) == 0) {
      if (e->numParametros != numArgs) {
        printf("Erro semântico: número incorreto de argumentos para '%s'. Esperado %d, encontrado %d\n", 
               nomeFuncao, e->numParametros, numArgs);
        return 0;
      }
      return 1;
    }
    
    // Verifica os filhos
    Escopo *filho = e->prox;
    while (filho != NULL) {
      if (strcmp(filho->nome, nomeFuncao) == 0) {
        if (filho->numParametros != numArgs) {
          printf("Erro semântico: número incorreto de argumentos para '%s'. Esperado %d, encontrado %d\n", 
                 nomeFuncao, filho->numParametros, numArgs);
          return 0;
        }
        return 1;
      }
      filho = filho->prox;
    }
    
    e = e->prox;
  }
  
  // Função não encontrada
  printf("Erro semântico: função '%s' não declarada\n", nomeFuncao);
  return 0;
}

// Verifica se um tipo de argumento é compatível com o tipo esperado do parâmetro
int verificarTipoArgumento(char *nomeFuncao, int indiceArg, char *tipoArg) {
  // Busca o escopo da função
  Escopo *e = escoposRaiz;
  while (e != NULL) {
    if (strcmp(e->nome, nomeFuncao) == 0) {
      if (indiceArg >= e->numParametros) {
        return 0; // Índice inválido
      }
      
      if (!tiposCompativeis(e->parametros[indiceArg].tipo, tipoArg)) {
        printf("Erro semântico: tipo incompatível para argumento %d de '%s'. Esperado '%s', encontrado '%s'\n", 
               indiceArg + 1, nomeFuncao, e->parametros[indiceArg].tipo, tipoArg);
        return 0;
      }
      return 1;
    }
    
    // Verifica os filhos
    Escopo *filho = e->prox;
    while (filho != NULL) {
      if (strcmp(filho->nome, nomeFuncao) == 0) {
        if (indiceArg >= filho->numParametros) {
          return 0; // Índice inválido
        }
        
        if (!tiposCompativeis(filho->parametros[indiceArg].tipo, tipoArg)) {
          printf("Erro semântico: tipo incompatível para argumento %d de '%s'. Esperado '%s', encontrado '%s'\n", 
                 indiceArg + 1, nomeFuncao, filho->parametros[indiceArg].tipo, tipoArg);
          return 0;
        }
        return 1;
      }
      filho = filho->prox;
    }
    
    e = e->prox;
  }
  
  return 0; // Função não encontrada
}
