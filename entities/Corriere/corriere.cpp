#include "corriere.h"
#include <cassert>

Corriere::Corriere(
    int id,
    int azienda,
    std::string nome,
    std::string cognome
){
    ID = id;
    Azienda = azienda;

    // La lunghezza del nome è di massimo 20 caratteri
    assert(nome.length() > 0 && nome.length() <= 20);
    NomeCorriere = nome;

    // La lunghezza del cognome è di massimo 20 caratteri
    assert(cognome.length() > 0 && cognome.length() <= 20);
    CognomeCorriere = cognome;
}