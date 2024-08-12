#RDIR punta alla cartella in cui sono i file di Redis
RDIR = ../../lib/con2redis/src
BDIR = ../bin #Path per la cartella bin (da creare)
ODIR = ../obj #Path per la cartella obj (da creare)

#CC indica il compilatore da utilizzare, nel nostro caso g++
CC = g++
#CFLAGS indica le flag utilizzate per la compilazione 
CFLAGS = -std=c++20 -lhiredis -lpq -I/usr/include/postgresql -I../../lib/con2db -I../../lib/con2redis

#DEPS contiene i file .h le cui dipendenze sono necessarie per eseguire il programma
DEPS = customer.h ../../lib/con2db/pgsql.h ../../lib/con2redis/src/redisfun.h

#Prende tutti i file nella cartella attuale con estensione .cpp)
SRC = $(wildcard *.cpp)

#Prende tutti i file oggetto nella cartella di Redis
con2redis_OBJ = $(widlcard ../../lib/con2redis/obj/*.o)

all: crea_Bin_Obj con2db con2redis

crea_Bin_Obj:
	mkdir -p $(BDIR) #Crea la cartella bin
	mkdir -p $(ODIR) #Crea la cartella obj
	
con2db:
	#Chiama il makefile in con2db
	$(MAKE) -C ../../lib/con2db #Attiva il Makefile
	
con2redis:
	#Chiama il makefile in con2redis
	$(MAKE) -C ../../lib/con2redis/src
# $(CC) = quello che sta scritto in CC. Scritto $(CC), mette il contenuto di CC e poi quello che segue
compile: $(CC)  -o my_program customer.cpp ../../lib/con2db/pgsql.cpp ../../lib/con2redis/src/redisfun.cpp $(CFLAGS)
