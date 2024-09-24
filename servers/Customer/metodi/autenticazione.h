#ifndef AUTENTICAZIONE_H
#define AUTENTICAZIONE_H

#include <string>
#include <iostream>
#include <sstream>
#include "../../../lib/con2db/pgsql.h"
#include "../../../lib/con2redis/src/con2redis.h"
#include "../../../lib/funzioniAusiliarie.h"
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>
#include <cassert>

#define WRITE_STREAM "CustomerW"
#define READ_STREAM "CustomerR"

#define REDIS_IP "localhost"
#define REDIS_PORT 6379

#define SERVER_PORT 5001

#define HOSTNAME "localhost"
#define DB_PORT "5432"
#define USERNAME "admin"
#define PASSWORD "admin"
#define DB_NAME "mewingdb"

int autentica(const char* mail);
bool creaCustomer(const char* email, const char* nome, const char* cognome, const char* via, int civico, const char* CAP, const char* city, const char* stato);

#endif //AUTENTICAZIONE_H
