#ifndef AUTENTICAZIONE_H
#define AUTENTICAZIONE_H

#include <string>
#include <iostream>
#include "../../../lib/con2db/pgsql.h"
#include "../../../lib/con2redis/src/con2redis.h"
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>
#include <cassert>

#define WRITE_STREAM "CustomerW"
#define READ_STREAM "CustomerR"

#define REDIS_IP "localhost"
#define REDIS_PORT 6379

#define SERVER_PORT 5000

#define HOSTNAME "localhost"
#define DB_PORT "5432"
#define USERNAME "admin"
#define PASSWORD "admin"
#define DB_NAME "mewingdb"

bool autentica(int clientSocket);

bool recuperaCustomer(Con2DB db, int clientSocket, const char* mail);

bool creaCustomer(Con2DB db, int clientSocket, const char* mail);

void inviaDati(int ID, const char* nome, const char* cognome, const char* mail, int abita); // Per leggibilità, l'invio dati viene gestito da un'unica funzione


#endif //AUTENTICAZIONE_H
