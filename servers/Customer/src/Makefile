IDIR=.
ODIR=../obj
BDIR=../bin

CC=g++
# Include Pistache libraries and other includes
CFLAGS=-std=c++20 -I. -I$(IDIR) -I/usr/include/postgresql -I/usr/local/include/pistache -lpq -lm -lhiredis
LDFLAGS=-L/usr/local/lib -lpistache -lhiredis -lpq -lm

DEPS = $(wildcard *.h) $(wildcard ../*.h)
SRC = $(wildcard *.cpp)
OBJ = $(patsubst %.cpp, $(ODIR)/%.o, $(notdir $(SRC)))

con2db_OBJ=$(wildcard ../../../lib/con2db/*.o)
con2redis_OBJ=$(wildcard ../../../lib/con2redis/src/*.o)
metodi_OBJ = $(wildcard ../metodi/*.o)
aux_OBJ = $(wildcard ../../../lib/*.o)

all: create_dirs con2db con2redis aux metodi $(BDIR)/customer_server

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
	$(MAKE) -C ../metodi # Calls the makefile for metodi

# Rule to compile *.cpp files from src into $(ODIR)/*.o
$(ODIR)/%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

# Rule to build the final executable
$(BDIR)/customer_server: $(OBJ) $(con2db_OBJ) $(con2redis_OBJ) $(aux_OBJ) $(metodi_OBJ)
	$(CC) -o $@ $(OBJ) $(con2db_OBJ) $(con2redis_OBJ) $(aux_OBJ) $(metodi_OBJ) $(LDFLAGS)

.PHONY: clean

clean:
	rm -rf $(ODIR) $(BDIR) ../../../lib/*.o ../metodi/*.o
