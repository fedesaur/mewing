IDIR=.
ODIR=../obj
BDIR=../bin

CC=g++
# use this when connecting to Postgresql (library lpq)
#CFLAGS=-std=c++11 -I. -I$(IDIR) -I/usr/include/postgresql -lpq -lm 
CFLAGS=-std=c++20 -I. -I$(IDIR) -I/usr/include/postgresql -lpq -lm -lhiredis -lpistache


# use this when no connection to Postgresql (library lpq) is needed
#CFLAGS=-std=c++11 -I. -I$(IDIR) -lm 

DEPS = $(wildcard $(IDIR)/*.h)
SRC = $(wildcard *.cpp)
OBJ = $(patsubst %.cpp, $(ODIR)/%.o, $(SRC))

con2db_OBJ=$(wildcard ../../../lib/con2db/*.o)
con2redis_OBJ=$(wildcard ../../../lib/con2redis/obj/*.o)
autentication_OBJ = $(wildcard ../metodi/*.o)
aux_OBJ = $(wildcard ../../../lib/*.o)


all:	create_dirs con2db con2redis aux autenticazione api courier_server $(BDIR)/main

create_dirs:
	mkdir -p $(BDIR)
	mkdir -p $(ODIR)

con2db:
	$(MAKE) -C ../../../lib/con2db

con2redis:
	$(MAKE) -C ../../../lib/con2redis/src
	
aux: ../../../lib/funzioniAusiliarie.cpp ../../../lib/funzioniAusiliarie.h
	$(CC) -c -o ../../../lib/funzioniAusiliarie.o $< $(CFLAGS)

autenticazione:
	$(MAKE) -C ../metodi #Chiama il makefile di autenticazione
	
api: routes.cpp routes.h ../../../lib/funzioniAusiliarie.h
	$(CC) -c -o $(ODIR)/routes.o $< $(CFLAGS)

courier_server: Courier_Server.cpp Courier_Server.h
	$(CC) -c -o $(ODIR)/Courier_Server.o $< $(CFLAGS)

$(BDIR)/main: $(OBJ) $(DEPS)
	$(CC) -o $@ $(OBJ) $(con2db_OBJ) $(con2redis_OBJ) $(aux_OBJ) $(autentication_OBJ) $(CFLAGS)

.PHONY: clean

clean:
	rm -r $(ODIR) $(BDIR) ../metodi/*.o
