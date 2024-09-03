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

std::pair<int, Prodotto*> recuperaForniti(int ID, Con2DB db, PGresult *res);
void mostraCarrello(int clientSocket, Prodotto* forniti, int righe);

#endif