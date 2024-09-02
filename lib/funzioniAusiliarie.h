#ifndef FUNZIONI_AUSILIARIE_H
#define FUNZIONI_AUSILIARIE_H

#include <string> // Importa std::string
#include "../entities/Prodotto.h" // Importa la struct Prodotto

bool isNumber(std::string stringa);
void rimuoviProdotto(int idP, Prodotto* insieme, int righe);

#endif