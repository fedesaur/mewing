IDIR=.

CC=g++
# use this when connecting to Postgresql (library lpq)
#CFLAGS=-std=c++11 -I. -I$(IDIR) -I/usr/include/postgresql -lpq -lm
CFLAGS=-std=c++20 -I. -I$(IDIR) -I/usr/include/postgresql -lpq -lm -lhiredis

# use this when no connection to Postgresql (library lpq) is needed
#CFLAGS=-std=c++11 -I. -I$(IDIR) -lm

con2db_OBJ=$(wildcard ../../../lib/con2db/*.o)
con2redis_OBJ=$(wildcard ../../../lib/con2redis/obj/*.o)


all:	con2db con2redis autenticazione prendiOrdine ricerca registraCorr rimuoviCorr recuperaCorr consegna ordiniCorrenti registro

con2db:
	$(MAKE) -C ../../../lib/con2db

con2redis:
	$(MAKE) -C ../../../lib/con2redis/src

autenticazione: autenticazione.cpp autenticazione.h ../../../lib/funzioniAusiliarie.h
	$(CC) -c -o autenticazione.o $< $(CFLAGS) # Crea il file autenticazione.o

storico: registroOrdini.cpp registroOrdini.h
	$(CC) -c -o registroOrdini.o $< $(CFLAGS) # Crea il file registroOrdini.o

ricerca:  ricercaOrdini.cpp ricercaOrdini.h 
	$(CC) -c -o ricercaOrdini.o $< $(CFLAGS) # Crea il file ricercaOrdini.o

prendiOrdine: prendiOrdine.cpp prendiOrdine.h
	$(CC) -c -o prendiOrdine.o $< $(CFLAGS) # Crea il file prendiOrdine.o

ordiniCorrenti: ordiniCorrenti.cpp ordiniCorrenti.h
	$(CC) -c -o ordiniCorrenti.o $< $(CFLAGS) # Crea il file ordiniCorrenti.o

registraCorr: registraCorriere.cpp registraCorriere.h
	$(CC) -c -o registraCorriere.o $< $(CFLAGS) # Crea il file registraCorriere.o

rimuoviCorr: rimuoviCorriere.cpp rimuoviCorriere.h
	$(CC) -c -o rimuoviCorriere.o $< $(CFLAGS) # Crea il file rimuoviCorriere.o

recuperaCorr: recuperaCorrieri.cpp recuperaCorrieri.h
	$(CC) -c -o recuperaCorrieri.o $< $(CFLAGS) # Crea il file recuperaCorrieri.o

consegna: consegnaOrdine.cpp consegnaOrdine.h
	$(CC) -c -o consegnaOrdine.o $< $(CFLAGS) # Crea il file consegnaOrdine.o

ordiniCorrenti: ordiniCorrenti.cpp ordiniCorrenti.h
	$(CC) -c -o ordiniCorrenti.o $< $(CFLAGS) # Crea il file ordiniCorrenti.o

registro: registroOrdini.cpp registroOrdini.h
	$(CC) -c -o registroOrdini.o $< $(CFLAGS) # Crea il file registroOrdini.o
.PHONY: clean

clean:
	rm -f *.o
