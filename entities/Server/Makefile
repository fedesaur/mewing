IDIR = . #Punta alla cartella attuale
BDIR = ./bin #Path per la cartella bin (da creare)
ODIR = ./obj #Path per la cartella obj (da creare)

CC = g++ #CC indica il compilatore da utilizzare, nel nostro caso g++
#CFLAGS indica le flag utilizzate per la compilazione 
CFLAGS = -std=c++20 -lhiredis -lpq -I/usr/include/postgresql -I -I$(IDIR)

DEPS = $(wildcard $(IDIR)/*.h) #DEPS contiene i file .h le cui dipendenze sono necessarie per eseguire il programma
SRC = $(wildcard *.cpp) #Prende tutti i file nella cartella attuale con estensione .cpp)
OBJ = $(patsubst %.cpp, $(ODIR)/%.o, $(SRC))

con2redis_OBJ = $(wildcard ../../lib/con2redis/obj/*.o) #File oggetto nella cartella di Redis
con2db_OBJ=$(wildcard ../../lib/con2db/*.o) #File oggetto nella cartella del DB

all: crea_Bin_Obj con2db con2redis $(ODIR)/%.o

crea_Bin_Obj:
	mkdir -p $(BDIR) #Crea la cartella bin
	mkdir -p $(ODIR) #Crea la cartella obj
	
con2db:
	$(MAKE) -C ../../lib/con2db #Attiva il Makefile #Chiama il makefile in con2db
	
con2redis:
	$(MAKE) -C ../../lib/con2redis/src	#Chiama il makefile in con2redis


# -c: Compila il file, ma non lo rende un eseguibile
#$@ = Nome del target
#$< = Nome della prima dipendenza (in questo caso, Customer.cpp)
$(ODIR)/%.o: Server.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

.PHONY: clean

clean:
	rmdir $(ODIR) $(BDIR)
