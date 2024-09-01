#ifndef COURIER_SERVER_H
#define COURIER_SERVER_H

#include <string>
#include <iostream>
#include "../../../lib/con2db/pgsql.h"
#include "../../../lib/con2redis/src/con2redis.h"
#include "../metodi/autenticazione.h"
#include "../metodi/ricercaProdotti.h"
#include "../../../entities/Customer.h" //da modificare
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>
#include <cassert>
#include <thread>
#include <mutex>

#define WRITE_STREAM "CourierW"
#define READ_STREAM "CourierR"
#define REDIS_IP "localhost"
#define REDIS_PORT 6379
#define SERVER_PORT 5002
#define MAX_CONNECTIONS 100 //Numero di connessioni massime accettabili


class Courier_Server
{
    private:
        // I parametri del Customer teniamoli privati per sicurezza
        redisContext *c2r; // c2r contiene le info sul contesto
        redisReply *reply; // reply contiene le risposte da Redis
        Customer CUSTOMER; // Struct che conserva le informazioni dell'utente attuale (da cambiare)
        int ID_CONNESSIONE = 0;
        int SERVER_SOCKET;
        std::mutex id_mutex;
        std::string OPZIONI[4]; // Opzioni dell'utente
        int NUMERO_OPZIONI = 4;
        bool handshake(int clientSocket);
        bool gestisciAutenticazione(int clientSocket);
        bool gestisciOperazioni(int clientSocket);
        void gestisciConnessioneCliente(int clientSocket, int connectionID);
    public:
        Courier_Server(); // Costruttore di Courier
        void gestisciConnessioni(); // Metodi di Courier
};



#endif //COURIER_SERVER_H
