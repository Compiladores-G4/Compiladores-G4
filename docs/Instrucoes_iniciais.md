# COMPILADORES


Para rodar o **Flex** e o **Bison** no Linux, siga estes passos:

### 1. Instalar o Flex e o Bison
Se ainda não tiver os pacotes instalados, use o seguinte comando para instalá-los:

```bash
sudo apt update
sudo apt install flex bison
```

Para distribuições baseadas em **Arch Linux**, use:

```bash
sudo pacman -S flex bison
```

Para **Fedora**:

```bash
sudo dnf install flex bison
```

---

### 2. Criar os arquivos de entrada

Crie dois arquivos:

- Um arquivo `.l` para o **Flex** (analisador léxico)
- Um arquivo `.y` para o **Bison** (analisador sintático)




### 3. Gerar e Compilar

1. **Gerar o analisador sintático com Bison**:

```bash
bison -d hello.y
```
Isso gera:
- `hello.tab.c` → Código C do analisador sintático
- `hello.tab.h` → Cabeçalho com definições dos tokens

2. **Gerar o analisador léxico com Flex**:

```bash
flex hello.l
```
Isso gera:
- `lex.yy.c` → Código C do analisador léxico

3. **Compilar tudo com o GCC**:

```bash
gcc -o analisador hello.tab.c lex.yy.c -lfl
```

---

### 4. Executar

Agora, você pode rodar o analisador:

```bash
./analisador < input.txt
```

---