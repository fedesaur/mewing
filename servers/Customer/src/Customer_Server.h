#ifndef CUSTOMER_SERVER_H
#define CUSTOMER_SERVER_H

#include <string>
#include <iostream>
#include <thread>
#include <mutex>
#include <pistache/endpoint.h>
#include <pistache/router.h>
#include "../../../lib/con2db/pgsql.h"
#include "../../../lib/con2redis/src/con2redis.h"
#include "../metodi/autenticazione.h"
#include "../metodi/gestisciCarrello.h"
#include "../metodi/modificaNome.h"
#include "../metodi/gestioneIndirizzi.h"
#include "../metodi/gestisciMetodi.h"
#include "routes.h"
#include <vector>
#include <cassert>

#define WRITE_STREAM "CustomerW"
#define READ_STREAM "CustomerR"
#define REDIS_IP "localhost"
#define REDIS_PORT 6379
#define SERVER_PORT 5001
#define MAX_CONNECTIONS 100 // Numero di connessioni massime accettabili

class Customer_Server
{
private:
    redisContext *c2r; // c2r contiene le info sul contesto
    redisReply *reply; // reply contiene le risposte da Redis
    int ID_CONNESSIONE = 0;
    int NUMERO_OPZIONI = 5;
    std::string OPZIONI[5]; // Opzioni dell'utente
    std::mutex id_mutex;
    

    // Funzioni per la gestione delle richieste HTTP con Pistache
    void defineRoutes();
    void handleOptions(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response); // Non statico

    // Funzioni per la connessione a Redis e gestione dei clienti
    bool handshake(int clientSocket);
    void gestisciConnessioneCliente(int clientSocket, int connectionID);
    bool gestisciAutenticazione(int clientSocket);
    bool gestisciOperazioni(int clientSocket);

public:
    Customer_Server(); // Costruttore di Customer
    ~Customer_Server(); // Distruttore di Customer
    std::thread pistacheThread; // Thread per Pistache server
    Pistache::Rest::Router router; // Router per Pistache
    void startPistache(); // Avvia il server Pistache
};

#endif // CUSTOMER_SERVER_H
