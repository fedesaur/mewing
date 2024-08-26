#ifndef CUSTOMER_H
#define CUSTOMER_H

#include <string>
#include <iostream>
#include "../../../lib/con2db/pgsql.h"
#include "../../../lib/con2redis/src/con2redis.h"
#include "../metodi/Autenticazione/autenticazione.h"
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>
#include <cassert>

#define CUSTOMER_STREAM "Customer"
#define REDIS_IP "localhost"
#define REDIS_PORT 6379
#define SERVER_PORT 5000
#define MAX_CONNECTIONS 100 //Numero di connessioni massime accettabili

/*
Questo converrà spostarlo poi
struct Indirizzo{
    std::string via;
    int civico;
    std::string cap;
    std::string city;
    std::string stato;
};
*/

class Customer{
	private:
		// I parametri del Customer teniamoli privati per sicurezza
		redisContext *c2r; // c2r contiene le info sul contesto
		redisReply *reply; // reply contiene le risposte da Redis
		int ID_CONNESSIONE = 0;
		int SERVER_SOCKET;
		bool handshake(int clientSocket);
		bool authenticate(int clientSocket);
    public:
        Customer(); // Costruttore di Customer
		void gestisciConnessioni(); // Metodi di Customer
};
#endif //CUSTOMER_H
