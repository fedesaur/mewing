#define customer_h

#include <string>
#include "../../lib/con2db/pgsql.h"
#include "../../lib/con2redis/src/con2redis.h"
#include <cassert>

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
            int prodID,
            int quantity
        );
        void AggiungiIndirizzo(
            std::string via,
            int civico,
            std::string cap,
            std::string city,
            std::string stato
        );
};