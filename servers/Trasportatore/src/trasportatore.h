#include <string>

class Trasportatore{
    public:
        int ID = 0;
        std::string IVA = NULL;
        std::string NomeTras = NULL;
        int Indirizzo = 0;
    
        Trasportatore(
            int id,
            std::string iva,
            std::string nome,
            int ind
        );
};