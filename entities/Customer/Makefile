IDIR = . #Punta alla cartella attuale
BDIR = ./bin #Path per la cartella bin (da creare)
ODIR = ./obj #Path per la cartella obj (da creare)

CC = g++ #CC indica il compilatore da utilizzare, nel nostro caso g++
#CFLAGS indica le flag utilizzate per la compilazione 
CFLAGS = -std=c++20 -lhiredis -lpq -I/usr/include/postgresql -I -I$(IDIR)

DEPS = $(wildcard *.h) #DEPS contiene i file .h le cui dipendenze sono necessarie per eseguire il programma
SRC = Customer.cpp #Prende tutti i file nella cartella attuale con estensione .cpp)
OBJ = $(patsubst %.cpp, $(ODIR)/%.o, $(SRC))

con2redis_OBJ = $(wildcard ../../lib/con2redis/obj/*.o) #File oggetto nella cartella di Redis
con2db_OBJ=$(wildcard ../../lib/con2db/*.o) #File oggetto nella cartella del DB
server_OBJ=$(wildcard ../Server/obj/Server.o) #File oggetto nella cartella del Server

#IMPORTANTE: Eseguire $(BDIR)/main ALLA FINE, dopo che sono stati avviati i vari make e creati i file oggetto
all: crea_Bin_Obj con2db con2redis server $(BDIR)/main

crea_Bin_Obj:
	mkdir -p $(BDIR) #Crea la cartella bin
	mkdir -p $(ODIR) #Crea la cartella obj
	
con2db:
	$(MAKE) -C ../../lib/con2db #Chiama il makefile in con2db
	
con2redis:
	$(MAKE) -C ../../lib/con2redis/src	#Chiama il makefile in con2redis
	
server:
	$(MAKE) -C ../Server #Chiama il makefile in server (ancora da fare)

#$@ = Nome del target
#$< = Nome della prima dipendenza (in questo caso, Customer.cpp)
#[N.B: Le dipendenze sono i file necessari per eseguire quel target e si scrivono sulla stessa riga del target]
$(ODIR)/Customer.o: Customer.cpp $(DEPS)
	$(CC) -o $@ -c $< $(CFLAGS)
	
$(BDIR)/main: $(OBJ) $(DEPS) #Crea in bin il main da eseguire
	$(CC) -o $@ $(OBJ) $(con2redis_OBJ) $(con2db_OBJ) $(server_OBJ) $(CFLAGS)
	
.PHONY: clean

clean:
	rmdir $(ODIR) $(BDIR) ../Server/bin ../Server/obj
