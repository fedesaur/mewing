#include "trasportatore.h"
#include <cassert>

Trasportatore::Trasportatore(
    int id,
    std::string iva,
    std::string nome,
    int ind
){
    ID = id;

    // La lunghezza dell'IVA è di 11 caratteri
    assert(iva.length() == 11);
    IVA = iva;

    // La lunghezza del Trasportatore è di massimo 100 caratteri
    assert(nome.length() > 0 && nome.length() <= 100);
    NomeTras = nome;
    
    Indirizzo = ind;
}