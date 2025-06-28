#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "tabela.h"
#include "gerador.h"  
#include "otimizador.h" 

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

  char *filename = argv[1];
  char *basename = strrchr(filename, '/');
  if (basename == NULL) {
    basename = strrchr(filename, '\\');
  }
  if (basename != NULL) {
    basename++; 
  } else {
    basename = filename;
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

  raiz = NULL;
  extern void inicializarCompilador();
  inicializarCompilador();
  
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
        imprimirCodigoIntermediario(codigo);
        
        char *dot = strrchr(basename, '.');
        if (dot != NULL) {
            *dot = '\0'; 
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
    
    int temFuncoes = 0;
    NoAST *atual = raiz;
    while (atual != NULL) {
        if (atual->tipo == NO_FUNCAO) {
            temFuncoes = 1;
            break;
        }
        atual = atual->proximoIrmao;
    }
    
    if (temFuncoes) {
        gerarCodigoC(raiz, output);
        
        fprintf(output, "\nint main() {\n");
        
        atual = raiz;
        while (atual != NULL) {
            if (atual->tipo == NO_FUNCAO) {
                if (strcmp(atual->nome, "main") != 0) { 
                    if (atual->esquerda == NULL) {
                        fprintf(output, "    %s();\n", atual->nome);
                    } else {
                    }
                }
            }
            atual = atual->proximoIrmao;
        }
        
        fprintf(output, "    return 0;\n");
        fprintf(output, "}\n");
    } else {
        fprintf(output, "int main() {\n");
        extern int nivelIndentacao;
        nivelIndentacao = 1;
        
        gerarCodigoC(raiz, output);
        
        nivelIndentacao = 0;
        
        fprintf(output, "    return 0;\n");
        fprintf(output, "}\n");
    }
    
    liberarAST(raiz);

    extern void liberarVariaveisDeclaradas();
    liberarVariaveisDeclaradas();
  }

  fclose(output);

  return 0;
}
