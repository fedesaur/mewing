#ifndef RICERCAPRODOTTI_H
#define RICERCAPRODOTTI_H

#include <string>
#include <iostream>
#include <sstream>
#include "../../../lib/con2db/pgsql.h"
#include "../../../lib/con2redis/src/con2redis.h"
#include "../../../entities/Prodotto.h"
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>
#include <cassert>

#define WRITE_STREAM "CustomerW"
#define READ_STREAM "CustomerR"

#define REDIS_IP "localhost"
#define REDIS_PORT 6379

#define SERVER_PORT 5000

#define HOSTNAME "localhost"
#define DB_PORT "5432"
#define USERNAME "admin"
#define PASSWORD "admin"
#define DB_NAME "mewingdb"

Prodotto *recuperaProdottiDisponibili(Con2DB db, PGresult *res, int clientSocket);
bool cercaProdottiDisponibili(int clientSocket);

#endif
