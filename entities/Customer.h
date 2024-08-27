#ifndef CUSTOMER_H
#define CUSTOMER_H

class Customer
{
    public:
        Customer(int id, const char* nome, const char* cognome, const char* mail,int abita);
        
    private:
        int ID;
        const char* nome;
        const char* cognome;
        const char* mail;
        int abita;
};

#endif //CUSTOMER_H
