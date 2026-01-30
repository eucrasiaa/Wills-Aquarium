# basic makefile

CC = gcc
CFLAGS = -Wall -Wextra -g
ADDRSAN = -fsanitize=address
LEAKSAN = -fsanitize=leak

all: program
program: main.o
	$(CC) $(CFLAGS) $(ADDRSAN) -o program main.o
main.o: main.c main.h art.h
