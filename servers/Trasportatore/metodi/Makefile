IDIR=.

CC=g++
# use this when connecting to Postgresql (library lpq)
#CFLAGS=-std=c++11 -I. -I$(IDIR) -I/usr/include/postgresql -lpq -lm
CFLAGS=-std=c++20 -I. -I$(IDIR) -I/usr/include/postgresql -lpq -lm -lhiredis

# use this when no connection to Postgresql (library lpq) is needed
#CFLAGS=-std=c++11 -I. -I$(IDIR) -lm

con2db_OBJ=$(wildcard ../../../lib/con2db/*.o)
con2redis_OBJ=$(wildcard ../../../lib/con2redis/obj/*.o)


all:	con2db con2redis autenticazione ricerca registraCorr rimuoviCorr recuperaCorr gestioneCorr

con2db:
	$(MAKE) -C ../../../lib/con2db

con2redis:
	$(MAKE) -C ../../../lib/con2redis/src

autenticazione: autenticazione.cpp autenticazione.h ../../../lib/funzioniAusiliarie.h
	$(CC) -c -o autenticazione.o $< $(CFLAGS) # Crea il file autenticazione.o

ricerca:  ricercaOrdini.cpp ricercaOrdini.h 
	$(CC) -c -o ricercaOrdini.o $< $(CFLAGS) # Crea il file ricercaOrdini.o

registraCorr: registraCorriere.cpp registraCorriere.h
	$(CC) -c -o registraCorriere.o $< $(CFLAGS) # Crea il file registraCorriere.o

rimuoviCorr: rimuoviCorriere.cpp rimuoviCorriere.h
	$(CC) -c -o rimuoviCorriere.o $< $(CFLAGS) # Crea il file rimuoviCorriere.o

recuperaCorr: recuperaCorrieri.cpp recuperaCorrieri.h
	$(CC) -c -o recuperaCorrieri.o $< $(CFLAGS) # Crea il file recuperaCorrieri.o

gestioneCorr: gestisciCorrieri.cpp gestisciCorrieri.h ../../../lib/funzioniAusiliarie.h
	$(CC) -c -o gestisciCorrieri.o $< $(CFLAGS) # Crea il file gestisciCorrieri.o
.PHONY: clean

clean:
	rm -f *.o
