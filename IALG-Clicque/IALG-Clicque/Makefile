
#Projekt IALG

PROGS=IALG-Clique
CC=gcc                              # prekladac jazyka C
CFLAGS=-Wall -std=c99 -pedantic -lm # parametry prekladace

all: $(PROGS)

$(PROGS): main.c check.c
	$(CC) $(CFLAGS) -o $@ main.c check.c

clean:
	rm -f *.o *.out $(PROGS)
#