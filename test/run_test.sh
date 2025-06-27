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

# Limpar arquivos anteriores
rm -f *.out *_intermediario.txt ../codigo_intermediario.txt

for testfile in *.py; do
    echo "Executando teste com $testfile..."
    
    # Executar o compilador e capturar toda a saída
    ../bin/compilador "$testfile" > "${testfile%.py}.out" 2>&1
    
    # O compilador agora gera arquivo específico com nome do teste
    expected_file="${testfile%.py}_intermediario.txt"
    
    # Se o arquivo específico não foi criado, verificar o arquivo global
    if [ ! -f "$expected_file" ] && [ -f ../codigo_intermediario.txt ]; then
        cp ../codigo_intermediario.txt "$expected_file"
    fi
    
    echo "Saída intermediária para $testfile:"
    if [ -f "$expected_file" ]; then
        cat "$expected_file"
    else
        echo "Erro: Arquivo de código intermediário não encontrado para $testfile"
    fi
    echo "-----------------------------"
    
    # Limpar arquivo global para próximo teste
    rm -f ../codigo_intermediario.txt
done

echo "Testes concluídos!"
