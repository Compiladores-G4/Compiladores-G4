#!/bin/bash

# Compilar o programa
make -C .. clean
make -C ..

# Verificar se o arquivo de entrada existe
if [ ! -f ../input.txt ]; then
    echo "Erro: input.txt não encontrado!"
    exit 1
fi

# Executar o programa com o arquivo de entrada
echo "Executando o compilador com input.txt..."
cd .. && ./compilador input.txt
