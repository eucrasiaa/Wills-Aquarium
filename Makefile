# basic makefile

CC = gcc
CFLAGS = -Wall -Wextra -g
ADDRSAN = -fsanitize=address
LEAKSAN = -fsanitize=leak


OBJ = main.o utils.o colors.o


all: aquarium
aq: aquarium 
	./aquarium
# main needs utils.o
# utils.o needs utils.h,c, colors.h,c, art.h
aquarium: $(OBJ)
	$(CC) $(CFLAGS) $(ADDRSAN) -o $@ $^
main.o: main.c utils.h art.h colors.o
	$(CC) $(CFLAGS) $(ADDRSAN) -c main.c

utils.o: utils.c utils.h colors.h art.h
	$(CC) $(CFLAGS) $(ADDRSAN) -c utils.c

colors.o: colors.c colors.h
	$(CC) $(CFLAGS) $(ADDRSAN) -c colors.c
# art.o: art.c art.h
# 	$(CC) $(CFLAGS) $(ADDRSAN) -c art.c
clean:
	rm -f *.o aquarium
