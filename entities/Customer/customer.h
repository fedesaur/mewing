#define customer_h

#include <string>
#include "../../lib/con2db/pgsql.h"
#include <cassert>
// Su VSC mi da errore con l'include (probabilmente perchè uso Windows)

struct Indirizzo{
    std::string via;
    int civico;
    std::string cap;
    std::string city;
    std::string stato;
};

class Customer{
    public:
        int ID = 0;
        std::string Nome = NULL;
        std::string Cognome = NULL;
        std::string Mail = NULL;
        int Abita = 0;

        Customer(int id,
        std::string nome,
        std::string cognome,
        std::string mail,
        int città);

        void AggiungiProdottoCarrello(
            Con2DB db1,
            int prodID,
            int quantity
        );
        void AggiungiIndirizzo(
            Con2DB db1,
            std::string via,
            int civico,
            std::string cap,
            std::string city,
            std::string stato
        );
};