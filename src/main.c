#include <stdio.h>
#include <stdlib.h>

#include "../obj/parser.tab.h"
#include "ast.h"
#include "tabela.h"

extern int yyparse(void);
extern FILE *yyin;
extern void yyrestart(FILE *);
extern NoAST *raiz;

int main(int arc, char **argv) {

  if (arc < 2) {
    fprintf(stderr, "Uso: %s <arquivo_de_entrada>\n", argv[0]);
    return 1;
  }

  FILE *input = fopen(argv[1], "r");
  if (input == NULL) {
    fprintf(stderr, "Erro ao abrir o arquivo de entrada: %s\n", argv[1]);
    return 1;
  }

  FILE *output = fopen("output.c", "w");
  if (output == NULL) {
    fprintf(stderr, "Erro ao abrir o arquivo de saída.\n");
    fclose(input);
    return 1;
  }

  yyin = input;

  yyparse();

  // Processar a AST gerada
  if (raiz != NULL) {
    fprintf(stdout, "AST gerada com sucesso!\n");
    imprimirASTDetalhada(raiz, 0);
  } else {
    fprintf(stderr, "Aviso: AST não foi gerada completamente\n");
  }

  fclose(input);
  fclose(output);
  imprimirTabela();

  // Liberar a memória utilizada pela AST
  if (raiz != NULL) {
    liberarAST(raiz);
  }

  return 0;
}
