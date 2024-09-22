#ifndef AGGIUNGI_FORNITO_H
#define AGGIUNGI_FORNITO_H

#include <string>
#include <utility> //Importa pair
#include <iostream>
#include <sstream>
#include "../../../lib/con2db/pgsql.h"
#include "../../../lib/con2redis/src/con2redis.h"
#include <unistd.h>
#include <cstring>

#define WRITE_STREAM "SupplierW"
#define READ_STREAM "SupplierR"

#define REDIS_IP "localhost"
#define REDIS_PORT 6379

#define SERVER_PORT 5002

#define HOSTNAME "localhost"
#define DB_PORT "5432"
#define USERNAMEP "producer"
#define PASSWORDP "producer"
#define DB_NAME "mewingdb"

bool aggiungiFornito(const char* email, const char* nomeProdotto, const char* descrizioneProdotto, double prezzoProdotto);

#endif
