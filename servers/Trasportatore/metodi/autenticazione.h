#ifndef AUTENTICAZIONE_H
#define AUTENTICAZIONE_H

#include <string>
#include <iostream>
#include <sstream>
#include "../../../lib/con2db/pgsql.h"
#include "../../../lib/con2redis/src/con2redis.h"
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>
#include <cassert>

#define WRITE_STREAM "CourierW"
#define READ_STREAM "CourierR"

#define REDIS_IP "localhost"
#define REDIS_PORT 6379

#define SERVER_PORT 5002

#define HOSTNAME "localhost"
#define DB_PORT "5432"
#define USERNAME "admin"
#define PASSWORD "admin"
#define DB_NAME "mewingdb"

bool autentica(int clientSocket);

bool recuperaTrasportatore(Con2DB db, int clientSocket, const char* piva);

bool creaTrasportatore(Con2DB db, int clientSocket, const char* piva);

bool inviaDati(int ID, const char* nome, const char* piva, int sede); // Per leggibilità, l'invio dati viene gestito da un'unica funzione


#endif //AUTENTICAZIONE_H
