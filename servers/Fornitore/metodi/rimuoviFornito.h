#ifndef RIMUOVI_FORNITO_H
#define RIMUOVI_FORNITO_H

#include <string>
#include <iostream>
#include <sstream>
#include <algorithm>
#include "../../../lib/con2db/pgsql.h"
#include "../../../lib/con2redis/src/con2redis.h"
#include <cstring>


#define SERVER_PORT 5002

#define HOSTNAME "localhost"
#define DB_PORT "5432"
#define USERNAMEP "producer"
#define PASSWORDP "producer"
#define DB_NAME "mewingdb"

bool rimuoviFornito(int supplierID, int productID);

#endif
