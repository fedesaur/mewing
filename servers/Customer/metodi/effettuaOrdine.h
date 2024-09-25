#ifndef EFFETTUA_ORDINE_H
#define EFFETTUA_ORDINE_H

#include <string>
#include <iostream>
#include <sstream>
#include <utility> //Importa pair
#include <algorithm>
#include "../../../lib/con2db/pgsql.h"
#include "../../../lib/con2redis/src/con2redis.h"
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
#define USERNAME_CUST "customer"
#define PASSWORD_CUST "customer"
#define DB_NAME "mewingdb"

bool effettuaOrdine(int customerID, const char* pagamento, int indirizzo);

#endif