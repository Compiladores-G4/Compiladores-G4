#ifndef TABELA_H
#define TABELA_H

#define TAM_NOME 64
#define TAM_TIPO 32
#define TAM_ESCOPO 64

typedef struct Simbolo {
  char nome[TAM_NOME];
  char tipo[TAM_TIPO];
  void *valor;
  char escopo[TAM_ESCOPO]; 
  int ehFuncao;             
  int numParametros;       
  char tipoRetorno[TAM_TIPO]; 
  struct Simbolo *prox;
} Simbolo;

typedef struct Escopo {
  char nome[TAM_ESCOPO];
  struct Escopo *pai;       
  struct Escopo *prox;     
} Escopo;

void inserirSimbolo(char *nome, char *tipo);
void inserirFuncao(char *nome, char *tipoRetorno, int numParametros);
Simbolo *buscarSimbolo(char *nome);
Simbolo *buscarFuncao(char *nome);
void imprimirTabela();
void atualizarSimboloValor(char *nome, void *valor);

void inicializarEscopos();
void criarEscopo(char *nome);
void sairEscopo();
Escopo *obterEscopoAtual();
char *obterNomeEscopoAtual();
void imprimirEscopos();
void destruirEscopos();

void inserirSimboloComEscopo(char *nome, char *tipo, char *escopo);
Simbolo *buscarSimboloNoEscopo(char *nome, char *escopo);
Simbolo *buscarSimboloCadeiaEscopos(char *nome); 

int verificarDeclaracao(char *nome);
int tiposCompativeis(char *tipo1, char *tipo2);
int verificarAtribuicao(char *destino, char *origem);
char* obterTipoResultante(char *tipo1, char *tipo2, char operador);
int verificarOperacao(char *nome1, char *nome2, char operador);

#endif // TABELA_H
