IDIR=.

CC=g++
# use this when connecting to Postgresql (library lpq)
#CFLAGS=-std=c++11 -I. -I$(IDIR) -I/usr/include/postgresql -lpq -lm
CFLAGS=-std=c++20 -I. -I$(IDIR) -I/usr/include/postgresql -lpq -lm -lhiredis

# use this when no connection to Postgresql (library lpq) is needed
#CFLAGS=-std=c++11 -I. -I$(IDIR) -lm

con2db_OBJ=$(wildcard ../../../lib/con2db/*.o)
con2redis_OBJ=$(wildcard ../../../lib/con2redis/obj/*.o)

all:	con2db con2redis autenticazione info forniti aggiungi modifica rimuovi

con2db:
	$(MAKE) -C ../../../lib/con2db

con2redis:
	$(MAKE) -C ../../../lib/con2redis/src

forniti: recuperaForniti.cpp recuperaForniti.h
	$(CC) -c $< $(CFLAGS) -o recuperaForniti.o # Crea il file recuperaForniti.o

info: modificaInfoF.cpp modificaInfoF.h ../../../lib/funzioniAusiliarie.h
	$(CC) -c $< $(CFLAGS) -o modificaInfoF.o # Crea il file modificaInfoF.o

aggiungi: aggiungiFornito.cpp aggiungiFornito.h
	$(CC) -c $< $(CFLAGS) -o aggiungiFornito.o # Crea il file aggiungiFornito.o

autenticazione: autenticazione.cpp autenticazione.h ../../../lib/funzioniAusiliarie.h
	$(CC) -c $< $(CFLAGS) -o autenticazione.o # Crea il file autenticazione.o

modifica: modificaFornito.cpp modificaFornito.h ../../../lib/funzioniAusiliarie.h
	$(CC) -c $< $(CFLAGS) -o modificaFornito.o # Crea il file modificaFornito.o

rimuovi: rimuoviFornito.cpp rimuoviFornito.h ../../../lib/funzioniAusiliarie.h
	$(CC) -c $< $(CFLAGS) -o rimuoviFornito.o # Crea il file rimuoviFornito.o

.PHONY: clean

clean:
	rm -f *.o
