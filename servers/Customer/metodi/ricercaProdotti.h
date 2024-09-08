#ifndef RICERCAPRODOTTI_H
#define RICERCAPRODOTTI_H

#include <string> // Importa std::string
#include <utility> //Importa pair
#include <iostream>
#include <sstream>
#include <algorithm>
#include "../../../lib/con2db/pgsql.h"
#include "../../../lib/con2redis/src/con2redis.h"
#include "../../../entities/Prodotto.h"
#include "../../../servers/Customer/metodi/recuperaCarrello.h"
#include "../../../lib/funzioniAusiliarie.h"
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
#define USERNAME_CUST "customer"
#define PASSWORD_CUST "customer"
#define DB_NAME "mewingdb"

bool aggiungiCarrelloDB(int idProdotto, int userID, int quantita, Con2DB db, PGresult *res);
int richiediQuantita(int clientSocket);
bool ricercaProdotti(int clientSocket);

#endif
