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

// Estrutura para parâmetros de função
typedef struct {
  char *nome;
  char *tipo;
} Parametro;

typedef struct Escopo {
  char nome[TAM_ESCOPO];
  struct Escopo *pai;       // Referência ao escopo pai para busca encadeada
  struct Escopo *prox;      // Próximo escopo no mesmo nível (para iteração)
  char *tipoRetorno;        // Tipo de retorno para funções
  Parametro *parametros;    // Lista de parâmetros para funções
  int numParametros;        // Número de parâmetros
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

// Funções para análise semântica de funções
/**
 * Define o tipo de retorno para o escopo atual (função)
 * @param tipo Tipo de retorno da função
 */
void definirTipoRetorno(char *tipo);

/**
 * Verifica se o tipo de retorno é compatível com o escopo atual
 * @param tipo Tipo do valor sendo retornado
 * @return 1 se compatível, 0 caso contrário
 */
int verificarRetorno(char *tipo);

/**
 * Adiciona parâmetros a um escopo de função
 * @param num Número de parâmetros
 * @param ... Lista de parâmetros (nome, tipo, nome, tipo, ...)
 */
void adicionarParametros(int num, ...);

/**
 * Verifica se uma chamada de função tem o número correto de argumentos
 * @param nomeFuncao Nome da função sendo chamada
 * @param numArgs Número de argumentos na chamada
 * @return 1 se correto, 0 caso contrário
 */
int verificarArgumentos(char *nomeFuncao, int numArgs);

/**
 * Verifica se um tipo de argumento é compatível com o tipo esperado do parâmetro
 * @param nomeFuncao Nome da função sendo chamada
 * @param indiceArg Índice do argumento (0-based)
 * @param tipoArg Tipo do argumento sendo passado
 * @return 1 se compatível, 0 caso contrário
 */
int verificarTipoArgumento(char *nomeFuncao, int indiceArg, char *tipoArg);

#endif // TABELA_H
