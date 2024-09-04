#ifndef RECUPERA_FORNITI_H
#define RECUPERA_FORNITI_H

#include <string>
#include <utility> //Importa pair
#include <iostream>
#include <sstream>
#include "../../../lib/con2db/pgsql.h"
#include "../../../lib/con2redis/src/con2redis.h"
#include "../../../entities/Prodotto.h"
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>

#define WRITE_STREAM "SupplierW"
#define READ_STREAM "SupplierR"

#define REDIS_IP "localhost"
#define REDIS_PORT 6379

#define HOSTNAME "localhost"
#define DB_PORT "5432"
#define USERNAME "producer"
#define PASSWORD "producer"
#define DB_NAME "mewingdb"

std::pair<int, Prodotto*> recuperaForniti();
void mostraForniti(int clientSocket, Prodotto* forniti, int righe);

#endif