#ifndef REGISTRO_ORDINI_H
#define REGISTRO_ORDINI_H

#include <string> // Importa std::string
#include <iostream>
#include <sstream>
#include "../../../lib/con2db/pgsql.h"
#include "../../../lib/con2redis/src/con2redis.h"
#include <cstring>

#define WRITE_STREAM "CourierW"
#define READ_STREAM "CourierR"

#define REDIS_IP "localhost"
#define REDIS_PORT 6379

#define SERVER_PORT 5003

#define HOSTNAME "localhost"
#define DB_PORT "5432"
#define USERNAME_TRAS "courier"
#define PASSWORD_TRAS "courier"
#define DB_NAME "mewingdb"

bool registroOrdini(const char* piva, int trasporterID);
#endif
