SRC     = src
INCLUDE = include
OBJ     = obj
BIN     = bin

BISON_FILE = $(SRC)/parser.y
FLEX_FILE  = $(SRC)/lexer.l

BISON_C = $(OBJ)/parser.tab.c
BISON_H = $(OBJ)/parser.tab.h
FLEX_C  = $(OBJ)/lexer.c

OBJS = $(OBJ)/main.o \
       $(OBJ)/parser.tab.o \
       $(OBJ)/lexer.o \
       $(OBJ)/ast.o \
       $(OBJ)/tabela.o

EXEC = $(BIN)/compilador

# Criação de diretórios automaticamente
$(shell mkdir -p $(OBJ) $(BIN))

# Regra padrão
all: $(EXEC)

# Geração dos arquivos do Bison
$(BISON_C) $(BISON_H): $(BISON_FILE)
	bison -d -o $(BISON_C) $(BISON_FILE)

# Geração do arquivo do Flex
$(FLEX_C): $(FLEX_FILE)
	flex -o $(FLEX_C) $(FLEX_FILE)

# Compilação dos arquivos .c
$(OBJ)/main.o: $(SRC)/main.c $(BISON_H)
	gcc -I$(INCLUDE) -I$(SRC) -c $< -o $@

$(OBJ)/parser.tab.o: $(BISON_C)
	gcc -I$(INCLUDE) -I$(SRC) -c $< -o $@

$(OBJ)/lexer.o: $(FLEX_C) $(BISON_H)
	gcc -I$(INCLUDE) -I$(SRC) -c $< -o $@

$(OBJ)/ast.o: $(SRC)/ast.c $(INCLUDE)/ast.h
	gcc -I$(INCLUDE) -I$(SRC) -c $< -o $@

$(OBJ)/tabela.o: $(SRC)/tabela.c $(INCLUDE)/tabela.h
	gcc -I$(INCLUDE) -I$(SRC) -c $< -o $@

# Linkagem final
$(EXEC): $(OBJS)
	gcc -o $(EXEC) $^ -lfl

# Limpeza
clean:
	rm -rf $(OBJ)/*.o $(OBJ)/*.c $(OBJ)/*.h $(EXEC)

# Execução do compilador
run: $(EXEC)
	./$(EXEC) input.txt

