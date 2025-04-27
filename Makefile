# Diretórios
SRC = src
OBJ = obj

# Arquivos do Bison e Flex
BISON_FILE = $(SRC)/parser.y
FLEX_FILE = $(SRC)/lexer.l

BISON_C = $(OBJ)/parser.tab.c
BISON_H = $(OBJ)/parser.tab.h
FLEX_C = $(OBJ)/lexer.c

# Executável final
EXEC = compilador

# Criar a pasta obj
$(shell mkdir -p $(OBJ))

# Regra principal
all: $(EXEC)

# Gerar o parser com Bison
$(BISON_C) $(BISON_H): $(BISON_FILE)
	bison -d -o $(BISON_C) $(BISON_FILE)

# Gerar o lexer com Flex
$(FLEX_C): $(FLEX_FILE)
	flex -d -o $(FLEX_C) $(FLEX_FILE)

# Compilar o compilador final
$(EXEC): $(BISON_C) $(FLEX_C)
	gcc -o $(EXEC) $(BISON_C) $(FLEX_C) -lfl

# Limpeza
clean:
	rm -rf $(OBJ)/*.o $(OBJ)/*.c $(OBJ)/*.h $(EXEC)

