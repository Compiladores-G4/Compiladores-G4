#!/bin/bash

# Compilar o programa
gcc -o analise parser.tab.c lex.yy.c -lfl

# Verificar se o arquivo de entrada existe
if [ ! -f input.txt ]; then
    echo "Erro: input.txt não encontrado!"
    exit 1
fi

# Executar o programa com o arquivo de entrada
echo "Executando analise com input.txt..."
./analise < input.txt
