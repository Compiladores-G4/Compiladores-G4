

# Documento Inicial – Linguagem Python para C

## Objetivo
Definir um subconjunto da linguagem Python que será aceito pelo compilador e como ele será transformado em C. Este documento inclui os tokens válidos, as estruturas sintáticas suportadas e exemplos de tradução.

---

## TOKENS

### Palavras-chave suportadas:

```
def, return, if, else, elif, while, for, in, range, True, False
```

### Identificadores:

- Letras (`a-z`, `A-Z`), underscore (`_`), seguidos de letras, números ou underscores.

### Operadores:
```
+  -  *  /  %  ==  !=  <  >  <=  >=  =  and  or  not
```

### Delimitadores:
```
( )  [ ]  { }  ,  :  ;  \n
```

### Tipos primitivos:
```
int, float, bool
```

### Literais:
- Inteiros: `123`
- Ponto flutuante: `3.14`
- Booleanos: `True`, `False`

---

## ESTRUTURAS DA LINGUAGEM SUPORTADAS

### 1. Funções

#### Python
```python
def soma(a: int, b: int) -> int:
    return a + b
```

#### C
```c
int soma(int a, int b) {
    return a + b;
}
```

---

### 2. Declaração de variáveis

#### Python
```python
x = 5
```

#### C
```c
int x = 5;
```

---

### 3. Condicionais

#### Python
```python
if x > 0:
    return True
else:
    return False
```

#### C
```c
if (x > 0) {
    return 1;
} else {
    return 0;
}
```

---

### 4. Laços de repetição - `while`

#### Python
```python
i = 0
while i < 10:
    i = i + 1
```

#### C
```c
int i = 0;
while (i < 10) {
    i = i + 1;
}
```

---

### 5. Laços de repetição - `for in range()`

#### Python
```python
for i in range(5):
    print(i)
```

#### C
```c
for (int i = 0; i < 5; i++) {
    printf("%d\n", i);
}
```

---

### 6. Operações booleanas

#### Python
```python
if x > 0 and y < 10:
    return True
```

#### C
```c
if (x > 0 && y < 10) {
    return 1;
}
```
---

## EXEMPLO COMPLETO

### Python
```python
def fatorial(n: int) -> int:
    resultado = 1
    while n > 1:
        resultado = resultado * n
        n = n - 1
    return resultado
```

### C
```c
int fatorial(int n) {
    int resultado = 1;
    while (n > 1) {
        resultado = resultado * n;
        n = n - 1;
    }
    return resultado;
}
```
