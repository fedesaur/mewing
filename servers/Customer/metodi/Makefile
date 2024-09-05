IDIR=.

CC=g++
CFLAGS=-std=c++20 -I. -I$(IDIR) -I/usr/include/postgresql -lpq -lm -lhiredis

con2db_OBJ=$(wildcard ../../../lib/con2db/*.o)
con2redis_OBJ=$(wildcard ../../../lib/con2redis/obj/*.o)

all: con2db con2redis autenticazione ricercaNormale modificaNome rimuoviDaCarrello recuperaCarrello

con2db:
	$(MAKE) -C ../../../lib/con2db

con2redis:
	$(MAKE) -C ../../../lib/con2redis/src

autenticazione: autenticazione.cpp autenticazione.h ../../../lib/funzioniAusiliarie.h
	$(CC) -c -o autenticazione.o $< $(CFLAGS)

ricercaNormale: ricercaProdotti.cpp ricercaProdotti.h ../../../lib/funzioniAusiliarie.h
	$(CC) -c -o ricercaProdotti.o $< $(CFLAGS)

modificaNome: modificaNome.cpp modificaNome.h
	$(CC) -c -o modificaNome.o $< $(CFLAGS)

rimuoviDaCarrello: rimuoviDaCarrello.cpp rimuoviDaCarrello.h
	$(CC) -c -o rimuoviDaCarrello.o $< $(CFLAGS)

recuperaCarrello: recuperaCarrello.cpp recuperaCarrello.h
	$(CC) -c -o recuperaCarrello.o $< $(CFLAGS)

.PHONY: clean

clean:
	rm -f *.o
