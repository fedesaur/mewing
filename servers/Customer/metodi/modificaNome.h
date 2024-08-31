#ifndef MODIFICA_NOME_H
#define MODIFICA_NOME_H

#include <string>
#include <utility> //Importa pair
#include <iostream>
#include <algorithm>
#include "../../../lib/con2db/pgsql.h"
#include "../../../lib/con2redis/src/con2redis.h"
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

bool modificaNome(int clientSocket);
std::pair<std::string,bool> cambiaNome(Con2DB db, PGresult *res, int clientSocket, int USER_ID);
std::pair<std::string,bool> cambiaCognome(Con2DB db, PGresult *res, int clientSocket, int USER_ID);
#endif