#include "buono.h"

// Costruttore del Buono
Buono::Buono(int id, time_t scadenza, float valoreB){
    ID = id;
    Scadenza = scadenza;
    Valore = valoreB;
}