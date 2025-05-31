#!/bin/bash

# Compila o projeto
echo "Compilando o projeto..."
cd ..
make clean
make

if [ $? -ne 0 ]; then
    echo "Erro na compilação. Abortando testes."
    exit 1
fi

# Executa o compilador com o arquivo de teste
echo "Executando teste com variables.py..."
./bin/compilador test/variables.py

echo "Teste concluído!"
