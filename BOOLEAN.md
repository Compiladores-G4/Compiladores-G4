# Implementação de Operações Booleanas Compostas

## Resumo

Este documento descreve a implementação de suporte para operações booleanas compostas (`and`, `or`, `not`) no compilador Python-like. Antes desta implementação, o compilador não suportava expressões como `if x > 0 and y < 10:`.

## O que foi implementado

### Operadores Suportados
- **`and`**: Operação lógica E entre duas expressões booleanas
- **`or`**: Operação lógica OU entre duas expressões booleanas  
- **`not`**: Operação lógica NÃO (negação) de uma expressão booleana

### Exemplos de uso
```python
# Operações básicas
result = x > 0 and y < 20
flag = a or b
negated = not condition

# Em estruturas condicionais
if x > 0 and y < 20:
    print("Válido")

if not finished:
    continue_processing()

# Expressões compostas
complex_condition = x > 0 and y < 20 or not finished
```

## Arquivos Modificados

### 1. `src/lexer.l`
**Mudanças realizadas:**
- Adicionados tokens para reconhecimento de `and`, `or`, `not`
- **CORREÇÃO CRÍTICA**: Movidas as definições dos operadores booleanos para ANTES da regra de identificadores genéricos

```lex
"and"                { return AND; }
"or"                 { return OR; }
"not"                { return NOT; }
```

**Motivo da correção**: O lexer estava reconhecendo `and`, `or`, `not` como identificadores comuns (`ID`) em vez dos tokens específicos, pois a regra `[A-Za-z_][A-Za-z0-9_]*` aparecia antes das palavras-chave.

### 2. `src/parser.y`
**Mudanças realizadas:**
- Adicionadas regras gramaticais para expressões booleanas compostas:
  ```yacc
  expr AND expr    // Operação AND
  expr OR expr     // Operação OR  
  NOT expr         // Operação NOT (unária)
  ```
- **Adicionadas regras de precedência** para resolver conflitos:
  ```yacc
  %left OR         // Menor precedência
  %left AND        
  %right NOT       // Maior precedência
  ```
- Mapeamento interno: `&` (AND), `|` (OR), `~` (NOT)

### 3. `include/gerador.h`
**Mudanças realizadas:**
- Adicionados novos tipos de operação no enum `TipoOperacao`:
  ```c
  OP_AND,     // and
  OP_OR,      // or  
  OP_NOT,     // not
  ```

### 4. `src/gerador.c`
**Mudanças realizadas:**
- Atualizada função `mapearOperador()` para incluir mapeamento dos novos operadores:
  ```c
  case '&': return OP_AND; // and
  case '|': return OP_OR;  // or
  case '~': return OP_NOT; // not
  ```
- Tratamento especial para operador unário NOT na função `gerarCodigoExpressao()`
- Adicionados casos na função `imprimirCodigoIntermediario()` para exibir os novos operadores:
  ```c
  case OP_AND: printf("%s = %s && %s\n", ...);
  case OP_OR:  printf("%s = %s || %s\n", ...);
  case OP_NOT: printf("%s = !%s\n", ...);
  ```

### 5. `src/tabela.c`
**Mudanças realizadas:**
- Atualizada função `verificarOperacao()` para validar tipos dos operadores booleanos:
  ```c
  // Operadores lógicos booleanos (AND, OR) só podem ser aplicados a bool
  if ((operador == '&' || operador == '|') && 
      (strcmp(s1->tipo, "bool") != 0 || strcmp(s2->tipo, "bool") != 0)) {
      // Erro semântico
  }
  ```

## Como Compilar

```bash
cd /home/harrymartins/Documentos/Compiladores-G4
make clean
make
```

## Como Testar

### Teste Básico
```bash
./bin/compilador test/simple_boolean.py
```

**Conteúdo do arquivo de teste:**
```python
x = 5
y = 10
result = x > 0 and y < 20
```

### Teste Completo
```bash
./bin/compilador test/boolean_operations.py
```

**Conteúdo do arquivo de teste:**
```python
x = 5
y = 10
a = True
b = False

if x > 0 and y < 20:
    result = True
else:
    result = False

if a or b:
    value = 1
else:
    value = 0

if not a:
    flag = False
else:
    flag = True

condition = x > 0 and y < 20 or not b
```

## Saída Esperada

### AST Gerada
```
Atribuição:
  Identificador: result
  Operador: &
    Operador: >
      Identificador: x
      Número: 0
    Operador: <
      Identificador: y
      Número: 20
```

### Código Intermediário
```
main:
    t5 = x
    t6 = 0
    t3 = t5 > t6
    t7 = y
    t8 = 20
    t4 = t7 < t8
    t2 = t3 && t4
    result = t2
end:
```

## Resolução de Problemas

### Conflitos de Precedência
- **Problema**: Conflitos shift/reduce no parser
- **Solução**: Adicionadas regras de precedência explícitas no parser.y

### Tokens não reconhecidos
- **Problema**: `and`, `or`, `not` sendo tratados como identificadores
- **Solução**: Reordenação das regras no lexer.l (palavras-chave antes de identificadores)

### Operador unário NOT
- **Problema**: NOT é unário, diferente de AND/OR que são binários
- **Solução**: Tratamento especial na geração de código com verificação de `operador == '~'`

## Análise Semântica

O compilador agora realiza verificação de tipos para operações booleanas:
- **AND/OR**: Requerem que ambos operandos sejam do tipo `bool`
- **NOT**: Requer que o operando seja do tipo `bool`
- Erros semânticos são reportados quando tipos incompatíveis são usados

## Status dos Conflitos

Após a implementação:
- **Antes**: 157 conflitos shift/reduce, 18 conflitos reduce/reduce
- **Depois**: 1 conflito shift/reduce, 18 conflitos reduce/reduce
- **Melhoria significativa** na resolução de conflitos gramaticais

## Testes de Validação

### ✅ Casos que funcionam:
- Expressões booleanas simples: `x > 0 and y < 20`
- Operador NOT: `not finished`
- Expressões em condicionais: `if a or b:`
- Expressões complexas: `x > 0 and y < 20 or not b`

### ✅ Análise semântica:
- Detecção de tipos incompatíveis
- Validação de operandos booleanos
- Inferência de tipos para resultados de operações

A implementação está **completa e funcional**, suportando todas as operações booleanas compostas conforme especificado.