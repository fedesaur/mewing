#include "funzioniAusiliarie.h"

bool isNumber(std::string stringa)
{
    for (int i = 0; stringa.length(); i++)
    {
        if (!std::isdigit(stringa[i])) return false;
    }
    return true;
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