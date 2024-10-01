#ifndef MODIFICA_FORNITO_H
#define MODIFICA_FORNITO_H

#include <string>
#include <iostream>
#include <sstream>
#include "../../../lib/con2db/pgsql.h"
#include "../../../lib/con2redis/src/con2redis.h"
#include <cstring>

#define SERVER_PORT 5002

#define HOSTNAME "localhost"
#define DB_PORT "5432"
#define USERNAMEP "producer"
#define PASSWORDP "producer"
#define DB_NAME "mewingdb"

bool modificaFornito(const char* nomeProdotto, const char* descrizioneProdotto, double prezzoProdotto, int productID);

#endif
