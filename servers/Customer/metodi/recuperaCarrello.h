#ifndef RECUPERA_CARRELLO_H
#define RECUPERA_CARRELLO_H

#include <string>
#include <utility> //Importa pair
#include <iostream>
#include <sstream>
#include <algorithm>
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

std::pair<int, Prodotto*> recuperaCarrello(int clientSocket);
void mostraCarrello(int clientSocket, Prodotto* carrello, int righe);

#endif
