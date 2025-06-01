CC = gcc
CFLAGS = -Wall -Wextra -I include

# Diretórios
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin
INCLUDE_DIR = include

# Arquivos fonte
SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRCS)) $(OBJ_DIR)/parser.tab.o $(OBJ_DIR)/lexer.o

# Executável final
TARGET = $(BIN_DIR)/compilador

# Regras
all: directories $(TARGET)

directories:
	mkdir -p $(OBJ_DIR) $(BIN_DIR)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ -lfl

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/parser.tab.o: $(OBJ_DIR)/parser.tab.c
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/lexer.o: $(OBJ_DIR)/lexer.c
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/parser.tab.c $(OBJ_DIR)/parser.tab.h: $(SRC_DIR)/parser.y
	bison -d -o $(OBJ_DIR)/parser.tab.c $<

$(OBJ_DIR)/lexer.c: $(SRC_DIR)/lexer.l $(OBJ_DIR)/parser.tab.h
	flex -o $@ $<

clean:
	rm -rf $(OBJ_DIR)/*.o $(BIN_DIR)/* $(OBJ_DIR)/parser.tab.c $(OBJ_DIR)/parser.tab.h $(OBJ_DIR)/lexer.c

.PHONY: all directories clean

