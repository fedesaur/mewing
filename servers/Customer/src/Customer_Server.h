#ifndef CUSTOMER_SERVER_H
#define CUSTOMER_SERVER_H

#include <string>
#include <iostream>
#include "../../../lib/con2db/pgsql.h"
#include "../../../lib/con2redis/src/con2redis.h"
#include "../metodi/autenticazione.h"
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


class Customer_Server
{
    private:
        // I parametri del Customer teniamoli privati per sicurezza
        redisContext *c2r; // c2r contiene le info sul contesto
        redisReply *reply; // reply contiene le risposte da Redis
        int ID_CONNESSIONE = 0;
        int SERVER_SOCKET;
        bool handshake(int clientSocket);
        bool authenticate(int clientSocket);
    public:
        Customer_Server(); // Costruttore di Customer
        void gestisciConnessioni(); // Metodi di Customer
};



#endif //CUSTOMER_SERVER_H
