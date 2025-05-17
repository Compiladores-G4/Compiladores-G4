SRC = src
OBJ = obj

BISON_FILE = $(SRC)/parser.y
FLEX_FILE = $(SRC)/lexer.l

BISON_C = $(OBJ)/parser.tab.c
BISON_H = $(OBJ)/parser.tab.h
FLEX_C = $(OBJ)/lexer.c

EXEC = compilador

$(shell mkdir -p $(OBJ))

all: $(EXEC)

$(BISON_C) $(BISON_H): $(BISON_FILE)
	bison -d -o $(BISON_C) $(BISON_FILE)

$(FLEX_C): $(FLEX_FILE)
	flex -o $(FLEX_C) $(FLEX_FILE)

$(EXEC): $(BISON_C) $(FLEX_C)
	gcc -Isrc -o $(EXEC) $(BISON_C) $(FLEX_C) $(SRC)/tabela.c $(SRC)/ast.c -lfl

clean:
	rm -rf $(OBJ)/*.o $(OBJ)/*.c $(OBJ)/*.h $(EXEC)

run: $(EXEC)
	./$(EXEC) input.txt

