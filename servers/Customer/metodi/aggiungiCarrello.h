#ifndef AGGIUNGI_CARRELLO_H
#define AGGIUNGI_CARRELLO_H

#include <string> // Importa std::string
#include <utility> //Importa pair
#include <iostream>
#include <sstream>
#include <algorithm>
#include "../../../lib/con2db/pgsql.h"
#include "../../../lib/con2redis/src/con2redis.h"
#include "../../../entities/Prodotto.h"
#include "../../../servers/Customer/metodi/recuperaCarrello.h"
#include "../metodi/recuperaProdotti.h"
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
#define USERNAME_CUST "admin"
#define PASSWORD_CUST "admin"
#define DB_NAME "mewingdb"

bool aggiungiCarrelloDB(int idProdotto, int userID, int quantita);
int richiediQuantita(int clientSocket);
bool aggiungiCarrello(int clientSocket, int customerID, Prodotto* prodotti, int RIGHE);

#endif
