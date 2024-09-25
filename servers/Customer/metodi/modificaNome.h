#ifndef MODIFICA_NOME_H
#define MODIFICA_NOME_H

#include <string>
#include <utility> // Importa pair
#include <iostream>
#include <algorithm>
#include "../../../lib/con2db/pgsql.h"
#include "../../../lib/con2redis/src/con2redis.h"
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>
#include <cassert>
#include <pistache/http.h>
#include <pistache/router.h>

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

bool modificaInfoCustomer(const char* email, const char* nome, const char* cognome);

#endif // MODIFICA_NOME_H
