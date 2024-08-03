#include "prodotto.h"
#include <cassert>

// Costruttore Prodotto
Prodotto::Prodotto(
    int id,
    std::string desc,
    double prz,
    Dimensioni dim,
    int forn,
    std::string nome
){
    ID = id;
    Descrizione = desc;
    
    // Il prezzo deve essere maggiore o uguale a 0
    assert(prz >= 0);
    Prezzo = prz;

    /* Le dimensioni devono essere maggiori di 0
    (Vediamo se cambiare qualcosa per oggi "bidimensionali"
    come i fogli di carta, ad esempio)
    */
    assert(dim.altezza > 0 && dim.larghezza > 0 && dim.lunghezza > 0);
    Dimensione = dim;
    
    FornitoreID = forn;

    // La lunghezza del nome deve essere massimo di 100 caratteri
    assert(nome.length() > 0 && nome.length() <= 100);
    NomeProd = nome;
}