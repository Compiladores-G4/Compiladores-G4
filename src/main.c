#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "tabela.h"
#include "gerador.h"  // Incluímos o cabeçalho do gerador
#include "otimizador.h" // Incluímos o cabeçalho do otimizador

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

  // Extrair o nome base do arquivo para usar nos arquivos de saída
  char *filename = argv[1];
  char *basename = strrchr(filename, '/');
  if (basename == NULL) {
    basename = strrchr(filename, '\\');
  }
  if (basename != NULL) {
    basename++; // Pular o caractere '/' ou '\\'
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

  // Reinicializar a AST e outras estruturas globais
  raiz = NULL;
  extern void inicializarCompilador();
  inicializarCompilador();
  
  // Adicionar debug para verificar se a AST está sendo corretamente reinicializada
  fprintf(stdout, "AST reinicializada: raiz = %p\n", (void*)raiz);
  
  yyin = input;

  int parse_result = yyparse();
  fprintf(stdout, "Após yyparse: raiz = %p\n", (void*)raiz);
  
  if (parse_result == 0 && raiz != NULL) {
    fprintf(stdout, "AST gerada com sucesso!\n");
    imprimirASTDetalhada(raiz, 0);
    
    if (erros_semanticos > 0) {
      fprintf(stderr, "\nForam encontrados %d erro(s) semântico(s).\n", erros_semanticos);
    } else {
      fprintf(stdout, "\nNenhum erro semântico encontrado!\n");
      fprintf(stdout, "Antes de gerar código intermediário: raiz = %p\n", (void*)raiz);
      
      CodigoIntermediario *codigo = gerarCodigoIntermediario(raiz);
      if (codigo != NULL) {
        fprintf(stdout, "\n===== CÓDIGO INTERMEDIÁRIO =====\n");
        imprimirCodigoIntermediario(codigo); // Imprime antes da otimização
        
        // Criar nome do arquivo de saída baseado no nome do arquivo de entrada
        char output_filename[256];
        char *dot = strrchr(basename, '.');
        if (dot != NULL) {
            *dot = '\0'; // Remover extensão
        }
        
        // Verificar se o nome já começa com "Teste_" para evitar duplicação
        if (strncmp(basename, "Teste_", 6) == 0) {
            snprintf(output_filename, sizeof(output_filename), "%s_intermediario.txt", basename);
        } else {
            snprintf(output_filename, sizeof(output_filename), "Teste_%s_intermediario.txt", basename);
        }
        
        FILE *codInterFile = fopen(output_filename, "w");
        if (codInterFile != NULL) {
          FILE *stdout_orig = stdout;
          stdout = codInterFile;
          imprimirCodigoIntermediario(codigo);
          stdout = stdout_orig; 
          fclose(codInterFile);
          
          fprintf(stdout, "Código intermediário salvo em: %s\n", output_filename);
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
    
    // Verificar se há funções definidas na AST
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
        // Se há funções definidas, gerar normalmente
        gerarCodigoC(raiz, output);
        
        // Gerar função main() automaticamente
        fprintf(output, "\nint main() {\n");
        
        // Percorrer a AST para encontrar funções definidas e gerar chamadas de teste
        atual = raiz;
        while (atual != NULL) {
            if (atual->tipo == NO_FUNCAO) {
                if (strcmp(atual->nome, "main") != 0) { 
                    
                    // Gerar chamada básica dependendo dos parâmetros
                    if (atual->esquerda == NULL) {
                        // Função sem parâmetros
                        fprintf(output, "    %s();\n", atual->nome);
                    } else {
                        // Função com parâmetros não será chamada automaticamente
                    }
                }
            }
            atual = atual->proximoIrmao;
        }
        
        fprintf(output, "    return 0;\n");
        fprintf(output, "}\n");
    } else {
        // Se não há funções, colocar todo o código dentro de main()
        fprintf(output, "int main() {\n");
        
        // Definir um nível de indentação inicial para o corpo da main
        extern int nivelIndentacao;
        nivelIndentacao = 1;
        
        gerarCodigoC(raiz, output);
        
        // Resetar indentação
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
