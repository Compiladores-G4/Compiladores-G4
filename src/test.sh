#!/bin/bash

# Verificar se foi fornecido um arquivo de teste
if [ $# -eq 0 ]; then
    echo "Uso: $0 <arquivo_de_teste>"
    echo "Exemplo: $0 teste_otimizacao.py"
    exit 1
fi

TEST_FILE=$1
TEST_PATH="../test/$TEST_FILE"

# Extrair o nome base do arquivo sem extensão
TEST_NAME=$(basename "$TEST_FILE" .py)

# Definir o nome do arquivo de saída para o código intermediário
# Verificar se o nome já começa com "Teste_" para evitar duplicação
if [[ "$TEST_NAME" == Teste_* ]]; then
    INTERMEDIATE_FILE="../${TEST_NAME}_intermediario.txt"
else
    INTERMEDIATE_FILE="../Teste_${TEST_NAME}_intermediario.txt"
fi

# Compilar o programa
make -C .. clean
make -C ..

# Verificar se o arquivo de teste existe
if [ ! -f "$TEST_PATH" ]; then
    echo "Erro: Arquivo de teste '$TEST_PATH' não encontrado!"
    echo "Arquivos disponíveis na pasta test:"
    ls -la ../test/
    exit 1
fi

# Executar o programa com o arquivo de teste
echo "Executando o compilador com $TEST_FILE..."
cd .. && ./bin/compilador "test/$TEST_FILE"

# Mostrar o código intermediário gerado
echo "\nCódigo intermediário gerado e salvo em $INTERMEDIATE_FILE:"
cat "$INTERMEDIATE_FILE"
