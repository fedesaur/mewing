IDIR=.
ODIR=../obj
BDIR=../bin

CC=g++
# Includere le librerie di Pistache
CFLAGS=-std=c++20 -I. -I$(IDIR) -I/usr/include/postgresql -lpq -lm -lhiredis -I/usr/local/include/pistache
LDFLAGS=-L/usr/local/lib -lpistache -lhiredis -lpq -lm

DEPS = $(wildcard $(IDIR)/*.h)
SRC = $(wildcard *.cpp)
OBJ = $(patsubst %.cpp, $(ODIR)/%.o, $(SRC))

con2db_OBJ=$(wildcard ../../../lib/con2db/*.o)
con2redis_OBJ=$(wildcard ../../../lib/con2redis/obj/*.o)
metodi_OBJ = $(wildcard ../metodi/*.o)
aux_OBJ = $(wildcard ../../../lib/*.o)

all: create_dirs con2db con2redis aux metodi customer_server $(BDIR)/main

create_dirs:
	mkdir -p $(BDIR)
	mkdir -p $(ODIR)

con2db:
	$(MAKE) -C ../../../lib/con2db

con2redis:
	$(MAKE) -C ../../../lib/con2redis/src

aux: ../../../lib/funzioniAusiliarie.cpp ../../../lib/funzioniAusiliarie.h
	$(CC) -c -o ../../../lib/funzioniAusiliarie.o $< $(CFLAGS)

metodi:
	$(MAKE) -C ../metodi #Chiama il makefile dei metodi

customer_server: Customer_Server.cpp Customer_Server.h
	$(CC) -c -o $(ODIR)/Customer_Server.o $< $(CFLAGS)

$(BDIR)/main: $(OBJ) $(DEPS)
	$(CC) -o $@ $(OBJ) $(con2db_OBJ) $(con2redis_OBJ) $(aux_OBJ) $(metodi_OBJ) $(LDFLAGS)

.PHONY: clean

clean:
	rm -r $(ODIR) $(BDIR) ../metodi/*.o
