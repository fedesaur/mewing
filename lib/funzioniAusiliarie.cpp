#include "funzioniAusiliarie.h"

bool isNumber(std::string s)
{
    std::string::const_iterator it = s.begin();
    while (it != s.end() && std::isdigit(*it)) ++it;
    return !s.empty() && it == s.end();
}
void rimuoviProdotto(int idP, Prodotto* insieme, int righe)
{
    int index = 0;
    while (index < righe)
    {
        if (insieme[index].ID == idP) break; //Rimuove dal carrello il prodotto con quell'ID
        index++;
    }
    
    while (index < righe) insieme[index] = insieme[index+1]; //Sposta tutti gli elementi di una posizione a sinistra
    return;
}