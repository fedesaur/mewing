IDIR = . #Punta alla cartella attuale
BDIR = ../bin #Path per la cartella bin (da creare)
ODIR = ../obj #Path per la cartella obj (da creare)

CC = g++ #CC indica il compilatore da utilizzare, nel nostro caso g++
#CFLAGS indica le flag utilizzate per la compilazione 
CFLAGS=-std=c++20 -I. -I$(IDIR) -I/usr/include/postgresql -lpq -lm -lhiredis

DEPS = $(wildcard $(IDIR)/*.h) #DEPS contiene i file .h le cui dipendenze sono necessarie per eseguire il programma
SRC = $(wildcard *.cpp) #Prende tutti i file nella cartella attuale con estensione .cpp)
OBJ = $(patsubst %.cpp, $(ODIR)/%.o, $(SRC))

con2redis_OBJ = $(wildcard ../../../lib/con2redis/obj/*.o) #File oggetto nella cartella di Redis
con2db_OBJ=$(wildcard ../../../lib/con2db/*.o) #File oggetto nella cartella del DB
server_OBJ=$(wildcard ../../Server/obj/*.o) #File oggetto nella cartella del Server

#IMPORTANTE: Eseguire $(BDIR)/main ALLA FINE, dopo che sono stati avviati i vari make e creati i file oggetto
all: crea_Bin_Obj con2db con2redis server $(ODIR)/$(OBJ) $(BDIR)/main

crea_Bin_Obj:
	mkdir -p $(BDIR) #Crea la cartella bin
	mkdir -p $(ODIR) #Crea la cartella obj
	
con2db:
	$(MAKE) -C ../../../lib/con2db/ #Chiama il makefile in con2db
	
con2redis:
	$(MAKE) -C ../../../lib/con2redis/src/ #Chiama il makefile in con2redis
	
server:
	$(MAKE) -C ../../Server/src/ #Chiama il makefile in Server

#$@ = Nome del target
#$< = Nome della prima dipendenza
#[N.B: Le dipendenze sono i file necessari per eseguire quel target e si scrivono sulla stessa riga del target]
$(ODIR)/%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)
	
$(BDIR)/main: $(OBJ) $(DEPS) #Crea in bin il main da eseguire
	$(CC) -o $@ $(OBJ) $(con2redis_OBJ) $(con2db_OBJ) $(server_OBJ) $(CFLAGS)
	
.PHONY: clean

clean:
	rm -r $(ODIR) $(BDIR) ../../Server/bin ../../Server/obj #Cancella bin e obj in Server
