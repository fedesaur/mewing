#ifndef AUTENTICAZIONE_H
#define AUTENTICAZIONE_H

#include <string>
#include <iostream>
#include "../../../../lib/con2db/pgsql.h"
#include "../../../../lib/con2redis/src/con2redis.h"
#include <cassert>

#define CUSTOMER_STREAM "Customer"

#define REDIS_IP "localhost"
#define REDIS_PORT 6379

#define SERVER_PORT 5000

#define HOSTNAME "localhost"
#define DB_PORT "5432"
#define USERNAME "customer"
#define PASSWORD "customer"
#define DB_NAME "mewingDB"


bool autentica(int IDConnessione);
bool controllaEsistenza(Con2DB db, std::string mail);

#endif //AUTENTICAZIONE_H
