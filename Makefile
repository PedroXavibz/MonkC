SHELL = /bin/bash

CC = cc
CFLAGS = -std=c11 -O3 -g -Wall -Wextra -Wpedantic -Wstrict-aliasing -lpcre
CFLAGS += -Wno-pointer-arith -Wno-newline-eof -Wno-unused-parameter -Wno-gnu-statement-expression
CFLAGS += -Wno-gnu-compound-literal-initializer -Wno-gnu-zero-variadic-macro-arguments

SRC = $(shell find ./src -name '*.c')
OBJ = $(SRC:.c=.o)
BIN = bin

all: dirs main

dirs:
	mkdir -p ./$(BIN)

main: $(OBJ)
	$(CC) $^ -o $(BIN)/main 

run: all
	$(BIN)/main

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@ 

clean:
	rm -rf $(BIN) $(OBJ)
