#ifndef CUSTOMER_H
#define CUSTOMER_H

#include <string>
#include <iostream>
#include "../../../lib/con2db/pgsql.h"
#include "../../../lib/con2redis/src/con2redis.h"
#include "../../Server/src/server.h"
#include <cassert>

#define READ_STREAM "CustomerIN"
#define WRITE_STREAM "CustomerOUT"
#define REDIS_IP "localhost"
#define REDIS_PORT 6379
#define SERVER_PORT 160
#define DB_PORT "160"
#define USERNAME "customer"
#define PASSWORD "customer"

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
		int ID; //ID viene generato dal database
        std::string Nome;
        std::string Cognome;
        std::string Mail;
        int Abita;
    public:
		// Costruttori di Customer
        Customer(std::string nome, std::string cognome, std::string mail, int città);
		// Metodi di Customer
        void AggiungiIndirizzo(
            std::string via,
            int civico,
            std::string cap,
            std::string city,
            std::string stato
        );
	    void CreateSocket();
};
#endif //CUSTOMER_H
