IDIR=.

CC=g++
CFLAGS=-std=c++20 -I. -I$(IDIR) -I/usr/include/postgresql -lpq -lm -lhiredis

con2db_OBJ=$(wildcard ../../../lib/con2db/*.o)
con2redis_OBJ=$(wildcard ../../../lib/con2redis/obj/*.o)

all: con2db con2redis autenticazione ricercaNormale modificaNome rimuoviDaCarrello recuperaCarrello gestisciCarrello recuperaIndirizzi aggiungiIndirizzo gestioneIndirizzi gestioneMetodi

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

gestisciCarrello: gestisciCarrello.cpp gestisciCarrello.h
	$(CC) -c -o gestisciCarrello.o $< $(CFLAGS)

recuperaIndirizzi: recuperaIndirizzi.cpp recuperaIndirizzi.h
	$(CC) -c -o recuperaIndirizzi.o $< $(CFLAGS)

aggiungiIndirizzo: aggiungiIndirizzo.cpp aggiungiIndirizzo.h ../../../lib/funzioniAusiliarie.h
	$(CC) -c -o aggiungiIndirizzo.o $< $(CFLAGS)

gestioneIndirizzi: gestioneIndirizzi.cpp gestioneIndirizzi.h ../../../lib/funzioniAusiliarie.h
	$(CC) -c -o gestioneIndirizzi.o $< $(CFLAGS)

gestioneMetodi: gestisciMetodi.cpp gestisciMetodi.h ../../../lib/funzioniAusiliarie.h
	$(CC) -c -o gestisciMetodi.o $< $(CFLAGS) 

.PHONY: clean

clean:
	rm -f *.o
