# Teste com todos os recursos implementados

# Declaração de função com parâmetros
def soma(a: int, b: int):
    return a + b

# Declaração de função sem parâmetros
def inicializar():
    x = 10
    y = 20
    return

# Função principal com laços
def main():
    # Variáveis
    contador = 0
    resultado = 0
    
    # Chamada de função
    inicializar()
    
    # Laço while
    while contador < 5:
        resultado = resultado + 1
        contador = contador + 1
    
    # Laço for
    for i in range(10):
        resultado = resultado + i
    
    # Chamada de função com parâmetros
    resultado = soma(5, 10)
    
    return