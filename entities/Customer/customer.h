#define customer_h

#include <string>

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
};