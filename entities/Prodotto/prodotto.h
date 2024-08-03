#include <string>

struct Dimensioni{
    float lunghezza;
    float larghezza;
    float altezza;
};

class Prodotto{
    public:
        int ID = 0;
        std::string Descrizione = NULL;
        double Prezzo = 0;
        Dimensioni Dimensione = {0,0,0};
        int FornitoreID = 0;
        std::string NomeProd = NULL;

        Prodotto(
            int id,
            std::string desc,
            double prz,
            Dimensioni dim,
            int forn,
            std::string nome
        );
};