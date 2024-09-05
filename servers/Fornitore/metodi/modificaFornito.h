#ifndef MODIFICA_FORNITO_H
#define MODIFICA_FORNITO_H

#include <string>
#include <utility> //Importa pair
#include <iostream>
#include <sstream>
#include <algorithm>
#include "../../../lib/con2db/pgsql.h"
#include "../../../lib/con2redis/src/con2redis.h"
#include "../../../entities/Prodotto.h"
#include "../metodi/recuperaForniti.h"
#include "../../../lib/funzioniAusiliarie.h"
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>

#define WRITE_STREAM "SupplierW"
#define READ_STREAM "SupplierR"

#define REDIS_IP "localhost"
#define REDIS_PORT 6379

#define SERVER_PORT 5001

#define HOSTNAME "localhost"
#define DB_PORT "5432"
#define USERNAMEP "producer"
#define PASSWORDP "producer"
#define DB_NAME "mewingdb"

bool modificaFornito(int clientSocket);
bool modificaAttributoFornito(Prodotto prodotto, int clientSocket);
std::pair<std::string,bool> cambiaNomeProdotto(int clientSocket, int idProdotto);
std::pair<std::string,bool> cambiaDescrizioneProdotto(int clientSocket, int idProdotto);
std::pair<std::string,bool> cambiaPrezzoProdotto(int clientSocket, int idProdotto);

#endif
