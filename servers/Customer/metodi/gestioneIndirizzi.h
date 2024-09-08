#ifndef GESTIONE_INDIRIZZI_H
#define GESTIONE_INDIRIZZI_H

#include <string>
#include <iostream>
#include <sstream>
#include <algorithm>
#include "../../../lib/con2db/pgsql.h"
#include "../../../lib/con2redis/src/con2redis.h"
#include "../metodi/recuperaIndirizzi.h"
#include "../metodi/aggiungiIndirizzo.h"
#include "../../../entities/Indirizzo.h"
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

#define SERVER_PORT 5000

#define HOSTNAME "localhost"
#define DB_PORT "5432"
#define USERNAME_CUST "customer"
#define PASSWORD_CUST "customer"
#define DB_NAME "mewingdb"

bool gestisciIndirizzi(int clientSocket);
int riceviIndice(int clientSocket, int righe);

#endif //AUTENTICAZIONE_H
