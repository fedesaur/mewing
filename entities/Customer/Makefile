#CC indica il compilatore da utilizzare, nel nostro caso g++
CC = g++
#CFLAGS indica le flag utilizzate per la compilazione 
CFLAGS = std=c++11 -lhiredis -lpq -I/usr/include/postgresql -I../../lib/con2db -I../../lib/con2redis

#DEPS contiene i file .h le cui dipendenze sono necessarie per eseguire il programma
DEPS = customer.h ../../lib/con2db/pgsql.h ../../lib/con2redis/src/redisfun.h

# $(CC) = quello che sta scritto in CC. Scritto $(CC), mette il contenuto di CC e poi quello che segue
compile: $(CC)  -o my_program customer.cpp ../../lib/con2db/pgsql.cpp ../../lib/con2redis/src/redisfun.cpp $(CFLAGS)
