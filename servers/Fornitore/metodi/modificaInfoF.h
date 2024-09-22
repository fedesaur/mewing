#ifndef MODIFICA_INFOF_H
#define MODIFICA_INFOF_H

#include <string>
#include <utility> //Importa pair
#include <iostream>
#include <algorithm>
#include "../../../lib/con2db/pgsql.h"
#include "../../../lib/con2redis/src/con2redis.h"
#include "../../../lib/funzioniAusiliarie.h"
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>
#include <cassert>

#define WRITE_STREAM "SupplierW"
#define READ_STREAM "SupplierR"

#define REDIS_IP "localhost"
#define REDIS_PORT 6379

#define SERVER_PORT 5002

#define HOSTNAME "localhost"
#define DB_PORT "5432"
#define USERNAMEP "producer"
#define PASSWORDP "producer"
#define DB_NAME "mewingdb"

bool modificaInfoF(int clientSocket);
std::pair<std::string,bool> cambiaNome(int clientSocket, int producerID);
std::pair<std::string,bool> cambiaIVA(int clientSocket, int producerID);
std::pair<std::string,bool> cambiaMail(int clientSocket, int producerID);
std::pair<std::string,bool> cambiaTelefono(int clientSocket, int producerID);
#endif
