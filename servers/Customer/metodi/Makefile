IDIR=.

CC=g++
# use this when connecting to Postgresql (library lpq)
#CFLAGS=-std=c++11 -I. -I$(IDIR) -I/usr/include/postgresql -lpq -lm
CFLAGS=-std=c++20 -I. -I$(IDIR) -I/usr/include/postgresql -lpq -lm -lhiredis

# use this when no connection to Postgresql (library lpq) is needed
#CFLAGS=-std=c++11 -I. -I$(IDIR) -lm

con2db_OBJ=$(wildcard ../../../lib/con2db/*.o)
con2redis_OBJ=$(wildcard ../../../lib/con2redis/obj/*.o)
aux_OBJ=../../../lib/funzioniAusiliarie.o

all: con2db con2redis aux autenticazione ricercaNormale modificaNome rimuoviDaCarrello recuperaCarrello

con2db:
	$(MAKE) -C ../../../lib/con2db

con2redis:
	$(MAKE) -C ../../../lib/con2redis/src

aux: ../../../lib/funzioniAusiliarie.cpp ../../../lib/funzioniAusiliarie.h
	$(CC) -c -o $(aux_OBJ) $< $(CFLAGS) # Crea il file ../../../lib/funzioniAusiliarie.o

autenticazione: autenticazione.cpp autenticazione.h $(aux_OBJ) $(con2db_OBJ) $(con2redis_OBJ)
	$(CC) -c -o autenticazione.o autenticazione.cpp $(CFLAGS) # Crea il file autenticazione.o
	$(CC) -o autenticazione autenticazione.o $(aux_OBJ) $(con2db_OBJ) $(con2redis_OBJ) $(CFLAGS) # Linka l'oggetto

ricercaNormale: ricercaProdotti.cpp ricercaProdotti.h $(aux_OBJ) $(con2db_OBJ) $(con2redis_OBJ)
	$(CC) -c -o ricercaProdotti.o ricercaProdotti.cpp $(CFLAGS) # Crea il file ricercaProdotti.o
	$(CC) -o ricercaNormale ricercaProdotti.o $(aux_OBJ) $(con2db_OBJ) $(con2redis_OBJ) $(CFLAGS) # Linka l'oggetto

modificaNome: modificaNome.cpp modificaNome.h $(aux_OBJ) $(con2db_OBJ) $(con2redis_OBJ)
	$(CC) -c -o modificaNome.o modificaNome.cpp $(CFLAGS) # Crea il file modificaNome.o
	$(CC) -o modificaNome modificaNome.o $(aux_OBJ) $(con2db_OBJ) $(con2redis_OBJ) $(CFLAGS) # Linka l'oggetto

rimuoviDaCarrello: rimuoviDaCarrello.cpp rimuoviDaCarrello.h $(aux_OBJ) $(con2db_OBJ) $(con2redis_OBJ)
	$(CC) -c -o rimuoviDaCarrello.o rimuoviDaCarrello.cpp $(CFLAGS) # Crea il file rimuoviDaCarrello.o
	$(CC) -o rimuoviDaCarrello rimuoviDaCarrello.o $(aux_OBJ) $(con2db_OBJ) $(con2redis_OBJ) $(CFLAGS) # Linka l'oggetto

recuperaCarrello: recuperaCarrello.cpp recuperaCarrello.h $(aux_OBJ) $(con2db_OBJ) $(con2redis_OBJ)
	$(CC) -c -o recuperaCarrello.o recuperaCarrello.cpp $(CFLAGS) # Crea il file recuperaCarrello.o
	$(CC) -o recuperaCarrello recuperaCarrello.o $(aux_OBJ) $(con2db_OBJ) $(con2redis_OBJ) $(CFLAGS) # Linka l'oggetto

.PHONY: clean

clean:
	rm autenticazione.o ricercaProdotti.o modificaNome.o rimuoviDaCarrello.o recuperaCarrello.o $(aux_OBJ)
