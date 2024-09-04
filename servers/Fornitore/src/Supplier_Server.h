#ifndef SUPPLIER_SERVER_H
#define SUPPLIER_SERVER_H

#include <string>
#include <iostream>
#include <utility> //Importa pair
#include <algorithm>
#include "../../../lib/con2db/pgsql.h"
#include "../../../lib/con2redis/src/con2redis.h"
#include "../metodi/autenticazione.h"
#include "../metodi/recuperaForniti.h"
#include "../metodi/aggiungiFornito.h"
#include "../metodi/rimuoviFornito.h"
#include "../metodi/modificaFornito.h"
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>
#include <cassert>
#include <thread>
#include <mutex>

#define WRITE_STREAM "SupplierW"
#define READ_STREAM "SupplierR"
#define REDIS_IP "localhost"
#define REDIS_PORT 6379
#define SERVER_PORT 5001
#define MAX_CONNECTIONS 100 //Numero di connessioni massime accettabili

class Supplier_Server
{
    private:
        // I parametri del Customer teniamoli privati per sicurezza
        redisContext *c2r; // c2r contiene le info sul contesto
        redisReply *reply; // reply contiene le risposte da Redis
        int ID_CONNESSIONE = 0;
        int SERVER_SOCKET;
        std::string OPZIONI[4]; // Opzioni dell'utente
        std::mutex id_mutex;
        int NUMERO_OPZIONI = 4;
        bool handshake(int clientSocket);
        void gestisciConnessioneCliente(int clientSocket, int connectionID);
        bool gestisciAutenticazione(int clientSocket);
        bool gestisciOperazioni(int clientSocket, int PRODUCER_ID);
    public:
        Supplier_Server(); // Costruttore di Customer
        void gestisciConnessioni(); // Metodi di Customer
};



#endif //SUPPLIER_SERVER_H
