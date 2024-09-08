#ifndef CUSTOMER_SERVER_H
#define CUSTOMER_SERVER_H

#include <string>
#include <iostream>
#include <algorithm>
#include "../../../lib/con2db/pgsql.h"
#include "../../../lib/con2redis/src/con2redis.h"
#include "../metodi/autenticazione.h"
#include "../metodi/gestisciCarrello.h"
#include "../metodi/modificaNome.h"
#include "../metodi/gestioneIndirizzi.h"
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>
#include <cassert>
#include <thread>
#include <mutex>

#define WRITE_STREAM "CustomerW"
#define READ_STREAM "CustomerR"
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
        int NUMERO_OPZIONI = 5;
        std::string OPZIONI[5]; // Opzioni dell'utente
        std::mutex id_mutex;
        bool handshake(int clientSocket);
        void gestisciConnessioneCliente(int clientSocket, int connectionID);
        bool gestisciAutenticazione(int clientSocket);
        bool gestisciOperazioni(int clientSocket);
    public:
        Customer_Server(); // Costruttore di Customer
        void gestisciConnessioni(); // Metodi di Customer
};



#endif //CUSTOMER_SERVER_H
