# Compiladores-G4

## 📋 Sobre o Projeto
Este projeto é um **compilador** que traduz código de um subconjunto da linguagem **Python para C**. Desenvolvido como projeto acadêmico para a disciplina de Compiladores, implementa todas as fases de um compilador: análise léxica, análise sintática, análise semântica e geração de código.

### Funcionalidades Suportadas

O compilador suporta as seguintes estruturas Python:

- **Variáveis e Tipos**: `int`, `float`, `bool`
- **Operações Aritméticas**: `+`, `-`, `*`, `/`, `%`
- **Operações Relacionais**: `==`, `!=`, `<`, `>`, `<=`, `>=`
- **Operações Booleanas**: `and`, `or`, `not`
- **Estruturas Condicionais**: `if`, `elif`, `else`
- **Laços de Repetição**: `while`, `for in range()`
- **Funções**: Definição com parâmetros tipados e valores de retorno
- **Listas**: Declaração e inicialização básica
- **Impressão**: Função `print()` com diferentes tipos

## Arquitetura do Compilador

### Componentes Principais

```
┌─────────────────┐    ┌─────────────────┐    ┌─────────────────┐
│  Código Python  │ →  │ Análise Léxica  │ →  │ Análise Sint.   │
│   (input.py)    │    │    (Flex)       │    │    (Bison)      │
└─────────────────┘    └─────────────────┘    └─────────────────┘
                                                       ↓
┌─────────────────┐    ┌─────────────────┐    ┌─────────────────┐
│   Código C      │ ←  │ Geração Código  │ ←  │ Análise Sem.    │
│  (output.c)     │    │                 │    │ + Tabela Símb.  │
└─────────────────┘    └─────────────────┘    └─────────────────┘
```

### Estrutura de Arquivos

```
Compiladores-G4/
├── src/                    # Código fonte
│   ├── lexer.l            # Analisador léxico (Flex)
│   ├── parser.y           # Analisador sintático (Bison)
│   ├── ast.c              # Árvore Sintática Abstrata
│   ├── tabela.c           # Tabela de símbolos e análise semântica
│   ├── gerador.c          # Geração de código intermediário e C
│   └── main.c             # Programa principal
├── include/               # Cabeçalhos
│   ├── ast.h
│   ├── tabela.h
│   ├── gerador.h
│   └── tipos.h
├── test/                  # Arquivos de teste
├── docs/                  # Documentação
├── bin/                   # Executável compilado
└── obj/                   # Arquivos objeto
```

### Fases de Compilação

1. **Análise Léxica**: Tokenização usando Flex
2. **Análise Sintática**: Parsing usando Bison, construção da AST
3. **Análise Semântica**: Verificação de tipos e escopo
4. **Geração de Código Intermediário**: Código de três endereços
5. **Geração de Código C**: Tradução final

## Como Executar o Projeto

### Pré-requisitos

Instale as dependências necessárias:

**Ubuntu/Debian:**
```bash
sudo apt update
sudo apt install flex bison gcc make
```

**Arch Linux:**
```bash
sudo pacman -S flex bison gcc make
```

### Compilação

1. **Clone e navegue para o diretório:**
```bash
cd /home/harrymartins/Documentos/Compiladores-G4
```

2. **Compile o projeto:**
```bash
make clean  # Limpa arquivos anteriores
make        # Compila o compilador
```

3. **Execute com um arquivo Python:**
```bash
./bin/compilador arquivo_teste.py
```

### Exemplo de Uso

**Entrada (teste.py):**
```python
def fatorial(n: int) -> int:
    resultado = 1
    while n > 1:
        resultado = resultado * n
        n = n - 1
    return resultado

x = 5
y = fatorial(x)
print(y)
```

**Comando:**
```bash
./bin/compilador teste.py
```

**Saída (output.c):**
```c
#include <stdio.h>
#include <stdbool.h>

int fatorial(int n) {
    int resultado = 1;
    while (n > 1) {
        resultado = resultado * n;
        n = n - 1;
    }
    return resultado;
}

int main() {
    int x = 5;
    int y = fatorial(x);
    printf("%d\n", y);
    return 0;
}
```

## Como Rodar os Testes

### Executar Todos os Testes

```bash
cd test
./run_test.sh
```

Este script:
1. Recompila o projeto automaticamente
2. Executa todos os arquivos `.py` no diretório `test/`
3. Gera arquivos de saída e código intermediário
4. Exibe os resultados de cada teste

### Executar Teste Individual

```bash
./bin/compilador test/Teste_Variaveis.py
```

### Arquivos de Teste Disponíveis

- `Teste_Variaveis.py` - Declaração de variáveis
- `Teste_Expressoes_mat.py` - Expressões matemáticas
- `Teste_OP_Booleanas.py` - Operações booleanas
- `Teste_Condicionais.py` - Estruturas if/elif/else
- `Teste_loop_while.py` - Laços while
- `Teste_LoopFor.py` - Laços for
- `Teste_Funcao*.py` - Definições de funções
- `Teste_Fatorial.py` - Exemplo completo
- `Teste_Lista.py` - Listas
- `Teste_Impressao.py` - Função print

### Limpeza dos Arquivos de Teste

Para remover arquivos gerados pelos testes:
```bash
cd test
rm -f *.out *_intermediario.txt output.c codigo_intermediario.txt
```

## Saídas do Compilador

O compilador gera três tipos de saída:

1. **AST (Árvore Sintática Abstrata)**: Representação estrutural do código
2. **Código Intermediário**: Representação de três endereços
3. **Código C**: Tradução final executável


## Características Técnicas

### Análise Léxica (Flex)
- Reconhecimento de tokens Python
- Tratamento de indentação com pilha
- Suporte a números inteiros e decimais
- Palavras-chave e operadores

### Análise Sintática (Bison)
- Gramática livre de contexto para Python
- Construção automática da AST
- Tratamento de precedência de operadores
- Resolução de conflitos shift/reduce

### Análise Semântica
- **Tabela de Símbolos**: Gerenciamento de escopos aninhados
- **Verificação de Tipos**: Compatibilidade entre operações
- **Verificação de Declarações**: Variáveis não declaradas
- **Inferência de Tipos**: Dedução automática de tipos

### Geração de Código
- **Código Intermediário**: Representação de três endereços
- **Otimizações Básicas**: Eliminação de redundâncias
- **Geração C**: Tradução para C padrão


## Desenvolvimento

### Makefile

```bash
make           # Compila o projeto
make clean     # Remove arquivos gerados
make all       # Equivalente a make
```

## 🤝 Contribuidores

<a href="https://github.com/Compiladores-G4/Compiladores-G4/graphs/contributors">
  <img src="https://contrib.rocks/image?repo=Compiladores-G4/Compiladores-G4" />
</a>


| Contribuidores | Matrícula | 
|----------|----------|
| Oscar Correia de Brito Neto  | 211031790  | 
| Leandro de Almeida Oliveira   | 211030827  | 
| Breno Queiroz Lima  | 211063069  | 
| Flavio Gustavo Araújo de Melo  | 211030602  |    
| Harryson Campos Martins  | 211039466  | 
| Yan Werlley de Freitas Paulo  | 211030649 | 
