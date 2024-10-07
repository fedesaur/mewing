#ifndef SUPPLIER_SERVER_H
#define SUPPLIER_SERVER_H

#include <string>
#include <utility> //Importa std::pair
#include <iostream>
#include "../../../lib/con2db/pgsql.h"
#include "../../../lib/con2redis/src/con2redis.h"
#include <pistache/endpoint.h>
#include <pistache/http.h>
#include <pistache/router.h>
#include <unistd.h>
#include <cstring>
#include <thread>
#include <mutex>
#include "routes.h"
#include <vector>
#include <chrono>

#define WRITE_STREAM "SupplierW"
#define READ_STREAM "SupplierR"
#define REDIS_IP "localhost"
#define REDIS_PORT 6379
#define SERVER_PORT 5002
#define MAX_CONNECTIONS 100 //Numero di connessioni massime accettabili

class Supplier_Server
{
    private:
        // I parametri del Customer teniamoli privati per sicurezza
        redisContext *c2r; // c2r contiene le info sul contesto
        redisReply *reply; // reply contiene le risposte da Redis
        int ID_CONNESSIONE = 0;
        std::mutex id_mutex;
        void defineRoutes();
    public:
        Supplier_Server(); // Costruttore di Courier
        ~Supplier_Server();
        std::thread pistacheThread; // Thread per Pistache server
        Pistache::Rest::Router router; // Router per Pistache
        void startPistache(); // Avvia il server Pistache
};



#endif //SUPPLIER_SERVER_H
