#include <string>

class Fornitore{
    public:
        int ID = 0;
        std::string Nome = NULL;
        std::string IVA = NULL;
        std::string Mail = NULL;
        std::string Telefono = NULL; //Meglio fare una stringa numerica che un intero
        int Sede = 0;

        Fornitore(
            int id,
            std::string nome,
            std::string iva,
            std::string mail,
            std::string tel,
            int sede
        );
};