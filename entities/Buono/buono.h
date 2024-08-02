#define buono_h

#include <time.h>

/* Classe Buono con i seguenti attributi
   NOTA: Bisogna controllare l'interazione con il database, poiché sarebbe comodo se,
   creando un nuovo buono nel database, creatore debba fornire solo il valore del buono
   mentre database generi in automatico ID e Scadenza
*/
class Buono{
    public: 
        int ID = 0;
        time_t Scadenza = NULL;
        float Valore = 0;

        Buono(int id, time_t scadenza, float valoreB);
};