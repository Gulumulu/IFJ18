# makefile for compiler
# team 37 - ximric01, xplsek03, xquirs00, xvarga14
# makefile implemented by xvarga14

CC=gcc
CFLAGS= -std=c99 -Wall -Wextra -pedantic 
SRC=scanner.c syntax.c predict.c errors.c symtable.c prectable.c semantic.c main.c queue.c
OUT=compiler

all:
	$(CC) $(CFLAGS) $(SRC) -o $(OUT)
clean:
	rm -f *.o


