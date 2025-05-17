#!/bin/bash

# Compilar o programa
gcc -o analise src/parser.tab.c obj/lexer.c src/tabela.c src/ast.c -Isrc -lfl

# Verificar se o arquivo de entrada existe
if [ ! -f input.txt ]; then
    echo "Erro: input.txt não encontrado!"
    exit 1
fi

# Executar o programa com o arquivo de entrada
echo "Executando analise com input.txt..."
./analise < input.txt
