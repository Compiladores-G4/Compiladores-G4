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

cd test

for testfile in *.py; do
    echo "Executando teste com $testfile..."
    ../bin/compilador "$testfile" > "${testfile%.py}.out"
    if [ -f ../codigo_intermediario.txt ]; then
        cp ../codigo_intermediario.txt "${testfile%.py}_intermediario.txt"
    fi
    echo "Saída intermediária para $testfile:"
    cat "${testfile%.py}_intermediario.txt"
    echo "-----------------------------"
done

echo "Testes concluídos!"
