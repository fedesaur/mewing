#ifndef REGISTRO_ORDINI_H
#define REGISTRO_ORDINI_H

#include <string> // Importa std::string
#include <tuple> // Importa std::tuple
#include <iostream>
#include <sstream>
#include <algorithm>
#include "../../../lib/con2db/pgsql.h"
#include "../../../lib/con2redis/src/con2redis.h"
#include "../../../entities/Ordine.h"
#include "../../../entities/Indirizzo.h"
#include "../../../lib/funzioniAusiliarie.h"
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>
#include <cassert>

#define WRITE_STREAM "CourierW"
#define READ_STREAM "CourierR"

#define REDIS_IP "localhost"
#define REDIS_PORT 6379

#define SERVER_PORT 5002

#define HOSTNAME "localhost"
#define DB_PORT "5432"
#define USERNAME "courier"
#define PASSWORD "courier"
#define DB_NAME "mewingdb"

std::tuple<int, Ordine*, Indirizzo*> registroOrdini(int clientSocket);
int ordinaOrdini(int RIGHE, Prodotto* ORDINI, Indirizzo* INDIRIZZI);
void mostraOrdini(int RIGHE, Prodotto* ORDINI, Indirizzo* INDIRIZZI);

#endif