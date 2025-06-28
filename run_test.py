#!/bin/python3

import subprocess

def obter_saida_programa(comando):
    """Executa o comando e retorna a saída (stdout) como string."""
    resultado = subprocess.run(comando, capture_output=True, text=True)
    return resultado.stdout.strip()

def ler_arquivo(caminho_arquivo):
    """Lê o conteúdo de um arquivo e retorna como string."""
    with open(caminho_arquivo, 'r') as f:
        return f.read().strip()

def comparar(comando, arquivo_txt):
    saida_programa = obter_saida_programa(comando)
    conteudo_arquivo = ler_arquivo(arquivo_txt)

    if saida_programa == conteudo_arquivo:
        print("✅ A saída do programa é igual ao conteúdo do arquivo.")
    else:
        print("❌ A saída do programa é diferente do conteúdo do arquivo.")
        print("\n--- Saída do Programa ---")
        print(saida_programa)
        print("\n--- Conteúdo do Arquivo ---")
        print(conteudo_arquivo)

import os
if __name__ == "__main__":
    test_files_path = "./test/input"
    test_files = [
        os.path.splitext(f)[0]
        for f in os.listdir(test_files_path)
        if os.path.isfile(os.path.join(test_files_path, f))
    ]


    for test_file in test_files:

        # with open(f"./test/expect/output_{test_file}.txt", 'w') as f:
        #     comando = ["./bin/compilador", f"./test/input/{test_file}.py"]
        #     result = obter_saida_programa(comando)
        #     f.write(result)

        comando = ["./bin/compilador", f"./test/input/{test_files[0]}.py"]
        comparar(comando, f"./test/expect/output_{test_files[0]}.txt")
