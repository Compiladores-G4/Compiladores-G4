def fatorial(n: int) -> int:
    resultado = 1
    while n > 1:
        resultado = resultado * n
        n = n - 1
    return resultado
