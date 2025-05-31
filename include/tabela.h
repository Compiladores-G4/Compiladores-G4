#ifndef TABELA_H
#define TABELA_H

#define TAM_NOME 64
#define TAM_TIPO 32
#define TAM_ESCOPO 64

typedef struct Simbolo {
  char nome[TAM_NOME];
  char tipo[TAM_TIPO];
  void *valor;
  char escopo[TAM_ESCOPO];  // Nome do escopo ao qual o símbolo pertence
  struct Simbolo *prox;
} Simbolo;

typedef struct Escopo {
  char nome[TAM_ESCOPO];
  struct Escopo *pai;       // Referência ao escopo pai para busca encadeada
  struct Escopo *prox;      // Próximo escopo no mesmo nível (para iteração)
} Escopo;

// Funções de manipulação de símbolos
void inserirSimbolo(char *nome, char *tipo);
Simbolo *buscarSimbolo(char *nome);
void imprimirTabela();
void atualizarSimboloValor(char *nome, void *valor);

// Funções de manipulação de escopos
void inicializarEscopos();
void criarEscopo(char *nome);
void sairEscopo();
Escopo *obterEscopoAtual();
char *obterNomeEscopoAtual();
void imprimirEscopos();
void destruirEscopos();

// Funções de símbolos com escopo
void inserirSimboloComEscopo(char *nome, char *tipo, char *escopo);
Simbolo *buscarSimboloNoEscopo(char *nome, char *escopo);
Simbolo *buscarSimboloCadeiaEscopos(char *nome); // Busca em cadeia de escopos

// Funções de análise semântica
int verificarDeclaracao(char *nome);
int tiposCompativeis(char *tipo1, char *tipo2);
int verificarAtribuicao(char *destino, char *origem);
char* obterTipoResultante(char *tipo1, char *tipo2, char operador);
int verificarOperacao(char *nome1, char *nome2, char operador);

#endif // TABELA_H
