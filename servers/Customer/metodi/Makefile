IDIR=.

CC=g++
CFLAGS=-std=c++20 -I. -I$(IDIR) -I/usr/include/postgresql -lpq -lm -lhiredis

con2db_OBJ=$(wildcard ../../../lib/con2db/*.o)
con2redis_OBJ=$(wildcard ../../../lib/con2redis/obj/*.o)

all: con2db con2redis autenticazione modificaNome recuperaProdotti recuperaCarrello aggiungiCarrello gestisciCarrello recuperaIndirizzi gestioneIndirizzi gestioneMetodi

con2db:
	$(MAKE) -C ../../../lib/con2db

con2redis:
	$(MAKE) -C ../../../lib/con2redis/src

autenticazione: autenticazione.cpp autenticazione.h ../../../lib/funzioniAusiliarie.h
	$(CC) -c -o autenticazione.o $< $(CFLAGS)

aggiungiCarrello: aggiungiCarrello.cpp aggiungiCarrello.h ../../../lib/funzioniAusiliarie.h
	$(CC) -c -o aggiungiCarrello.o $< $(CFLAGS)

modificaNome: modificaNome.cpp modificaNome.h
	$(CC) -c -o modificaNome.o $< $(CFLAGS)

recuperaProdotti: recuperaProdotti.cpp recuperaProdotti.h ../../../lib/funzioniAusiliarie.h
	$(CC) -c -o recuperaProdotti.o $< $(CFLAGS)

recuperaCarrello: recuperaCarrello.cpp recuperaCarrello.h
	$(CC) -c -o recuperaCarrello.o $< $(CFLAGS)

gestisciCarrello: gestisciCarrello.cpp gestisciCarrello.h ../../../lib/funzioniAusiliarie.h
	$(CC) -c -o gestisciCarrello.o $< $(CFLAGS)

recuperaIndirizzi: recuperaIndirizzi.cpp recuperaIndirizzi.h
	$(CC) -c -o recuperaIndirizzi.o $< $(CFLAGS)

gestioneIndirizzi: gestioneIndirizzi.cpp gestioneIndirizzi.h ../../../lib/funzioniAusiliarie.h
	$(CC) -c -o gestioneIndirizzi.o $< $(CFLAGS)

gestioneMetodi: gestisciMetodi.cpp gestisciMetodi.h ../../../lib/funzioniAusiliarie.h
	$(CC) -c -o gestisciMetodi.o $< $(CFLAGS) 

.PHONY: clean

clean:
	rm -f *.o
