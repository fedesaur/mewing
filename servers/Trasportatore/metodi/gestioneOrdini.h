#ifndef GESTISCI_ORDINI_H
#define GESTISCI_ORDINI_H

#include <string> // Importa std::string
#include <utility> // Importa std::pair
#include <tuple> // Importa std::tuple
#include <iostream>
#include <sstream>
#include <algorithm>
#include "../../../lib/con2db/pgsql.h"
#include "../../../lib/con2redis/src/con2redis.h"
#include "../../../lib/funzioniAusiliarie.h"
#include "../../../entities/Ordine.h"
#include "../../../entities/Indirizzo.h"
#include "../../../entities/Corriere.h"
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
#define USERNAME_TRAS "courier"
#define PASSWORD_TRAS "courier"
#define DB_NAME "mewingdb"

bool gestioneOrdini(int clientSocket);
#endif