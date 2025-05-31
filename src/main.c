#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../obj/parser.tab.h"
#include "ast.h"
#include "tabela.h"

extern int yyparse(void);
extern FILE *yyin;
extern void yyrestart(FILE *);
extern NoAST *raiz;
extern int erros_semanticos;

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

  printf("Analisando arquivo: %s\n", argv[1]);
  
  FILE *output = fopen("output.c", "w");
  if (output == NULL) {
    fprintf(stderr, "Erro ao abrir o arquivo de saída.\n");
    fclose(input);
    return 1;
  }

  yyin = input;

  // Adicionar maior controle de erro
  int parse_result = yyparse();
  
  if (parse_result == 0 && raiz != NULL) {
    fprintf(stdout, "AST gerada com sucesso!\n");
    imprimirASTDetalhada(raiz, 0);
    
    // Verificar erros semânticos
    if (erros_semanticos > 0) {
      fprintf(stderr, "\nForam encontrados %d erro(s) semântico(s).\n", erros_semanticos);
    } else {
      fprintf(stdout, "\nNenhum erro semântico encontrado!\n");
    }
  } else {
    fprintf(stderr, "Erro: AST não foi gerada corretamente. Código de erro: %d\n", parse_result);
    fprintf(stderr, "Verifique a sintaxe do seu arquivo de entrada.\n");
  }

  fclose(input);
  fclose(output);
  imprimirTabela();
  imprimirEscopos();

  if (raiz != NULL) {
    liberarAST(raiz);
  }

  return 0;
}
