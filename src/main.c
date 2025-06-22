#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "tabela.h"
#include "gerador.h"  // Incluímos o cabeçalho do gerador

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

  int parse_result = yyparse();
  
  if (parse_result == 0 && raiz != NULL) {
    fprintf(stdout, "AST gerada com sucesso!\n");
    imprimirASTDetalhada(raiz, 0);
    
    if (erros_semanticos > 0) {
      fprintf(stderr, "\nForam encontrados %d erro(s) semântico(s).\n", erros_semanticos);
    } else {
      fprintf(stdout, "\nNenhum erro semântico encontrado!\n");
      
      CodigoIntermediario *codigo = gerarCodigoIntermediario(raiz);
      if (codigo != NULL) {
        fprintf(stdout, "Código intermediário gerado com sucesso!\n");
        imprimirCodigoIntermediario(codigo);
        
        FILE *codInterFile = fopen("codigo_intermediario.txt", "w");
        if (codInterFile != NULL) {
          FILE *stdout_orig = stdout;
          stdout = codInterFile;
          imprimirCodigoIntermediario(codigo);
          stdout = stdout_orig; 
          fclose(codInterFile);
        }
        
        liberarCodigoIntermediario(codigo);
      }
    }
  } else {
    fprintf(stderr, "Erro: AST não foi gerada corretamente. Código de erro: %d\n", parse_result);
    fprintf(stderr, "Verifique a sintaxe do seu arquivo de entrada.\n");
  }

  fclose(input);

  imprimirTabela();
  imprimirEscopos();

  if (raiz != NULL) {
    fprintf(output, "#include <stdio.h>\n");
    fprintf(output, "#include <stdbool.h>\n\n");
    
    gerarCodigoC(raiz, output);
    liberarAST(raiz);

    extern void liberarVariaveisDeclaradas();
    liberarVariaveisDeclaradas();
  }

  fclose(output);

  return 0;
}
