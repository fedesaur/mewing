#ifndef RICERCAORDINI_H
#define RICERCAORDINI_H

#include <string> // Importa std::string
#include <utility> //Importa pair
#include <iostream>
#include <sstream>
#include <algorithm>
#include "../../../lib/con2db/pgsql.h"
#include "../../../lib/con2redis/src/con2redis.h"
#include "../../../entities/Ordine.h"
#include "../../../lib/funzioniAusiliarie.h"
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>
#include <cassert>
#include <ctime>

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

bool ricercaOrdini(int clientSocket);
std::pair<int, Prodotto*> recuperaProdottiDisponibili(int userID, Con2DB db, PGresult *res, int clientSocket);
bool aggiungiAlCarrello(Con2DB db, PGresult *res, int USER_ID, std::pair<int, Prodotto*> carrello, std::pair<int, Prodotto*> disponibili, int clientSocket);
bool aggiungiCarrelloDB(int idProdotto, int userID, int quantita, Con2DB db, PGresult *res);


#endif
