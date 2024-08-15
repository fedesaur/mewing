#ifndef CUSTOMER_H
#define CUSTOMER_H

#include <thread>
#include <string>
#include <iostream>
#include "../../lib/con2db/pgsql.h"
#include "../../lib/con2redis/src/con2redis.h"
#include "../Server/Server.h"
#include <cassert>

#define READ_STREAM "CustomerIN"
#define WRITE_STREAM "CustomerOUT"
#define REDIS_IP "localhost"
#define REDIS_PORT 6379

struct Indirizzo{
    std::string via;
    int civico;
    std::string cap;
    std::string city;
    std::string stato;
};

class Customer{
	private:
		// I parametri del Customer teniamoli privati per sicurezza
		int ID = 0; //ID viene generato dal database
        std::string Nome = NULL;
        std::string Cognome = NULL;
        std::string Mail = NULL;
        int Abita = 0;
    public:
		// Costruttore di Customer
        Customer(std::string nome, std::string cognome, std::string mail, int città);

        void AggiungiIndirizzo(
            std::string via,
            int civico,
            std::string cap,
            std::string city,
            std::string stato
        );
};
#endif //CUSTOMER_H
