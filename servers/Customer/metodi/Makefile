IDIR=.

CC=g++
# use this when connecting to Postgresql (library lpq)
#CFLAGS=-std=c++11 -I. -I$(IDIR) -I/usr/include/postgresql -lpq -lm
CFLAGS=-std=c++20 -I. -I$(IDIR) -I/usr/include/postgresql -lpq -lm -lhiredis

# use this when no connection to Postgresql (library lpq) is needed
#CFLAGS=-std=c++11 -I. -I$(IDIR) -lm

con2db_OBJ=$(wildcard ../../../lib/con2db/*.o)
con2redis_OBJ=$(wildcard ../../../lib/con2redis/obj/*.o)

all:	con2db con2redis autenticazione ricerca modificaNome

con2db:
	$(MAKE) -C ../../../lib/con2db

con2redis:
	$(MAKE) -C ../../../lib/con2redis/src

autenticazione: autenticazione.cpp autenticazione.h 
	$(CC) -c -o autenticazione.o $< $(CFLAGS) # Crea il file autenticazione.o

ricerca:  ricercaProdotti.cpp ricercaProdotti.h 
	$(CC) -c -o ricercaProdotti.o $< $(CFLAGS) # Crea il file ricercaProdotti.o

modificaNome: modificaNome.cpp modificaNome.h
	$(CC) -c -o modificaNome.o $< $(CFLAGS) # Crea il file modificaNome.o

.PHONY: clean

clean:
	rm autenticazione.o ricercaProdotti.o modificaNome.o
