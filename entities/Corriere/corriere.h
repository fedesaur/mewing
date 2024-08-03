#include <string>

class Corriere{
    public:
        int ID = 0;
        int Azienda = 0;
        std::string NomeCorriere = NULL;
        std::string CognomeCorriere = NULL;

        Corriere(
            int id,
            int azienda,
            std::string nome,
            std::string cognome
        );
};