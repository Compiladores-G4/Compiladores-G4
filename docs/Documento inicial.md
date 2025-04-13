# Compilador C → Python

## Objetivo

Desenvolver um compilador capaz de traduzir programas escritos em uma subconjunto robusto da linguagem C para programas equivalentes em Python 3, com suporte a estruturas fundamentais da linguagem, escopos, funções, expressões, laços e controle de fluxo.

---

## Tokens Reconhecidos

### Palavras-chave
- Tipos: `int`, `float`, `char`, `void`
- Controle de fluxo: `if`, `else`, `while`, `for`, `break`, `continue`, `return`
- Entrada e saída: `printf`, `scanf`

### Operadores
- Aritméticos: `+`, `-`, `*`, `/`, `%`
- Relacionais: `==`, `!=`, `<`, `>`, `<=`, `>=`
- Atribuição: `=`, `+=`, `-=`, `*=`, `/=`
- Lógicos: `&&`, `||`, `!`
- Incremento/decremento: `++`, `--`

### Delimitadores
- `(`, `)`, `{`, `}`, `[`, `]`, `;`, `,`

### Identificadores e Literais
- Identificadores: `[a-zA-Z_][a-zA-Z0-9_]*`
- Literais:
  - Inteiros: `[0-9]+`
  - Flutuantes: `[0-9]+\.[0-9]+`
  - Strings: `"[^"]*"`
  - Caracteres: `'.'`

### Comentários
- Linha única: `// ...`
- Bloco: `/* ... */`

---

## Estruturas Suportadas

### Tipos de dados
- `int`, `float`, `char`, `void`
- Suporte básico a arrays unidimensionais: `int arr[10];`

### Declarações e escopo
- Variáveis globais e locais
- Atribuições com operadores compostos

### Funções
- Declaração e definição
- Argumentos e retorno
- Chamadas de funções entre si

### Controle de fluxo
- Condicionais: `if`, `else if`, `else`
- Laços: `while`, `for`, com `break`, `continue`
- `return`

### Expressões
- Aritméticas, relacionais, lógicas e chamadas de função dentro de expressões

### Entrada e saída
- `printf` → mapeado para `print()`
- `scanf` → mapeado para `input()` com conversão

---

## Tradução C → Python (Exemplos)

### Função com parâmetros e retorno

**Código C:**
```c
int soma(int a, int b) {
    return a + b;
}
```

**Código Python:**
```python
def soma(a, b):
    return a + b
```

---

### Laço `for` com incremento

**Código C:**
```c
for (int i = 0; i < 10; i++) {
    printf("%d\n", i);
}
```

**Código Python:**
```python
for i in range(10):
    print(i)
```

---

### Função principal e controle de fluxo

**Código C:**
```c
int main() {
    int x = 5;
    if (x > 0) {
        printf("positivo\n");
    } else {
        printf("negativo\n");
    }
    return 0;
}
```

**Código Python:**
```python
def main():
    x = 5
    if x > 0:
        print("positivo")
    else:
        print("negativo")

main()
```

---

### Entrada de dados

**Código C:**
```c
int x;
scanf("%d", &x);
```

**Código Python:**
```python
x = int(input())
```

---

### Array unidimensional (uso básico)

**Código C:**
```c
int nums[3] = {1, 2, 3};
```

**Código Python:**
```python
nums = [1, 2, 3]
```
