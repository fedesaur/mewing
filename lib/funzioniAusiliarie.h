#ifndef FUNZIONI_AUSILIARIE_H
#define FUNZIONI_AUSILIARIE_H

#include <string> // Importa std::string
#include "../entities/Prodotto.h" // Importa la struct Prodotto
#include <algorithm> //Importa erase e std::remove
#include <netinet/in.h> //Importa il necessario per gestire i socket
#include <sys/socket.h>

bool isNumber(std::string stringa);
void rimuoviProdotto(int idP, Prodotto* insieme, int righe);
int riceviIndice(int clientSocket, int righe);

#endif