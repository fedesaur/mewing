IDIR=.

CC=g++
CFLAGS=-std=c++20 -I. -I$(IDIR) -I/usr/include/postgresql -lpq -lm -lhiredis

con2db_OBJ=$(wildcard ../../../lib/con2db/*.o)
con2redis_OBJ=$(wildcard ../../../lib/con2redis/obj/*.o)

all: con2db con2redis autenticazione modificaNome recuperaProdotti aggiungiIndirizzo recuperaCarrello rimuoviCarrello aggiungiCarrello effettuaOrdine annullaOrdine recuperaIndirizzi recuperaOrdini

con2db:
	$(MAKE) -C ../../../lib/con2db

con2redis:
	$(MAKE) -C ../../../lib/con2redis/src

autenticazione: autenticazione.cpp autenticazione.h ../../../lib/funzioniAusiliarie.h
	$(CC) -c -o autenticazione.o $< $(CFLAGS)

aggiungiIndirizzo: aggiungiIndirizzo.cpp aggiungiIndirizzo.h
	$(CC) -c -o aggiungiIndirizzo.o $< $(CFLAGS)

aggiungiCarrello: aggiungiCarrello.cpp aggiungiCarrello.h
	$(CC) -c -o aggiungiCarrello.o $< $(CFLAGS)

modificaNome: modificaNome.cpp modificaNome.h
	$(CC) -c -o modificaNome.o $< $(CFLAGS)

rimuoviCarrello: rimuoviCarrello.cpp rimuoviCarrello.h
	$(CC) -c -o rimuoviCarrello.o $< $(CFLAGS)

recuperaProdotti: recuperaProdotti.cpp recuperaProdotti.h ../../../lib/funzioniAusiliarie.h
	$(CC) -c -o recuperaProdotti.o $< $(CFLAGS)

recuperaCarrello: recuperaCarrello.cpp recuperaCarrello.h
	$(CC) -c -o recuperaCarrello.o $< $(CFLAGS)

effettuaOrdine: effettuaOrdine.cpp effettuaOrdine.h
	$(CC) -c -o effettuaOrdine.o $< $(CFLAGS)

annullaOrdine: annullaOrdine.cpp annullaOrdine.h
	$(CC) -c -o annullaOrdine.o $< $(CFLAGS)

recuperaOrdini: recuperaOrdini.cpp recuperaOrdini.h
	$(CC) -c -o recuperaOrdini.o $< $(CFLAGS)

recuperaIndirizzi: recuperaIndirizzi.cpp recuperaIndirizzi.h
	$(CC) -c -o recuperaIndirizzi.o $< $(CFLAGS)

.PHONY: clean

clean:
	rm -f *.o
