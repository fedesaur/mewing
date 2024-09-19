#ifndef COURIER_SERVER_H
#define COURIER_SERVER_H

#include <string>
#include <utility> //Importa std::pair
#include <iostream>
#include "../../../lib/con2db/pgsql.h"
#include "../../../lib/con2redis/src/con2redis.h"
#include "../metodi/autenticazione.h"
#include "../metodi/gestioneOrdini.h"
#include "../metodi/gestisciCorrieri.h"
#include "../metodi/registroOrdini.h"
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
        int ID_CONNESSIONE = 0;
        int SERVER_SOCKET;
        std::mutex id_mutex;
        std::string OPZIONI[3]; // Opzioni dell'utente
        int NUMERO_OPZIONI = 3;
        bool handshake(int clientSocket);
        bool gestisciAutenticazione(int clientSocket);
        bool gestisciOperazioni(int clientSocket);
        void gestisciConnessioneCliente(int clientSocket, int connectionID);
    public:
        Courier_Server(); // Costruttore di Courier
        void gestisciConnessioni(); // Metodi di Courier
};



#endif //COURIER_SERVER_H
