#ifndef AUTENTICAZIONE_H
#define AUTENTICAZIONE_H

// Usato per contenere i vari import
#include <string>
#include <iostream>
#include "../../../../lib/con2db/pgsql.h"
#include "../../../../lib/con2redis/src/con2redis.h"
#include <cassert>

#define READ_STREAM "CustomerIN"
#define WRITE_STREAM "CustomerOUT"
#define REDIS_IP "localhost"
#define REDIS_PORT 6379
#define SERVER_PORT 5000
#define DB_PORT "5432"
#define USERNAME "customer"
#define PASSWORD "customer"

bool autentica();

#endif //AUTENTICAZIONE_H
