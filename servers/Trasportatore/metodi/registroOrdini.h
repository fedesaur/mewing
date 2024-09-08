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
#include <optional>

#define WRITE_STREAM "CourierW"
#define READ_STREAM "CourierR"

#define REDIS_IP "localhost"
#define REDIS_PORT 6379

#define SERVER_PORT 5002

#define HOSTNAME "localhost"
#define DB_PORT "5432"
#define USERNAMEC "courier"
#define PASSWORDC "courier"
#define DB_NAME "mewingdb"

std::optional<std::tuple<int, Ordine*, Indirizzo*>> registroOrdini(int clientSocket);
int ordinaOrdini(int RIGHE, Ordine* ORDINI, Indirizzo* INDIRIZZI);
void mostraOrdini(int RIGHE, Ordine* ORDINI, Indirizzo* INDIRIZZI);

#endif
