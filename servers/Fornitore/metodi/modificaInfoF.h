#ifndef MODIFICA_INFOF_H
#define MODIFICA_INFOF_H

#include <string>
#include <iostream>
#include <algorithm>
#include "../../../lib/con2db/pgsql.h"
#include "../../../lib/con2redis/src/con2redis.h"
#include <unistd.h>
#include <cstring>
#include <cassert>

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

bool modificaInfoF(const char* email, const char* nome, const char* IVA, const char* telefono);
#endif
