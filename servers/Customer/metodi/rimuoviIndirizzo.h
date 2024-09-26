#ifndef AGGIUNGI_INDIRIZZO_H
#define AGGIUNGI_INDIRIZZO_H

#include <string> // Importa std::string
#include "../../../lib/con2db/pgsql.h"
#include "../../../lib/con2redis/src/con2redis.h"
#include <cstring>

#define WRITE_STREAM "CustomerW"
#define READ_STREAM "CustomerR"

#define REDIS_IP "localhost"
#define REDIS_PORT 6379

#define SERVER_PORT 5001

#define HOSTNAME "localhost"
#define DB_PORT "5432"
#define USERNAME_CUST "customer"
#define PASSWORD_CUST "customer"
#define DB_NAME "mewingdb"

bool rimuoviIndirizzo(int userID, int addressID);

#endif