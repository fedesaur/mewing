#include "routes.h"
#include "../metodi/autenticazione.h"
#include "../metodi/recuperaProdotti.h"
#include <pistache/http.h>
#include <pistache/endpoint.h>
#include <pistache/router.h>
#include <iostream>

void defineRoutes(Pistache::Rest::Router& router) {
    // Registrazione delle rotte con funzioni globali
    Pistache::Rest::Routes::Post(router, "/autentica/:email", Pistache::Rest::Routes::bind(&authenticateUser));
    Pistache::Rest::Routes::Post(router, "/modificaNome", Pistache::Rest::Routes::bind(&modificaNomeHttp));
    Pistache::Rest::Routes::Get(router, "/prodotti", Pistache::Rest::Routes::bind(&getProdotti));
}


void authenticateUser(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response) {
    // Recupera l'email dal percorso
    auto email = request.param(":email").as<std::string>();

    if (email.empty()) {
        response.send(Pistache::Http::Code::Bad_Request, "Email not provided");
        return;
    }

    // Connetti a Redis
    redisContext *c2r = redisConnect(REDIS_IP, REDIS_PORT);
    if (c2r == nullptr || c2r->err) {
        response.send(Pistache::Http::Code::Internal_Server_Error, "Unable to connect to Redis");
        return;
    }

    // Prepara il comando per aggiungere l'email allo stream
    redisReply* reply = static_cast<redisReply*>(redisCommand(c2r, "XADD %s * email %s", WRITE_STREAM, email.c_str()));

    // Controlla l'esito del comando Redis
    if (reply == nullptr || reply->type != REDIS_REPLY_STRING) {
        response.send(Pistache::Http::Code::Internal_Server_Error, "Error writing email to Redis stream");
        redisFree(c2r);
        return;
    }

    // Pulisci la risposta di Redis
    freeReplyObject(reply);
    redisFree(c2r);

    // Simulazione del socket client
    int clientSocket = 0;

    // Ora chiama la funzione autentica
    bool autenticato = autentica(clientSocket);

    if (autenticato) {
        response.send(Pistache::Http::Code::Ok, "User authenticated");
    } else {
        response.send(Pistache::Http::Code::Unauthorized, "Authentication failed");
    }
}


void modificaNomeHttp(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response) {
    // Implementa la logica di modifica del nome qui
    response.send(Pistache::Http::Code::Ok, "Name updated");
}

void getProdotti(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response) {
    // Recupera tutti i prodotti disponibili
    std::pair<int, Prodotto*> risultato = recuperaProdotti(0); // Ignoriamo il clientSocket

    // Costruisci la risposta
    if (risultato.first > 0 && risultato.second != nullptr) {
        std::stringstream ss;
        ss << "PRODOTTI DISPONIBILI:\n";

        // Itera sui prodotti e li inserisce nella stringa di risposta
        for (int i = 0; i < risultato.first; ++i) {
            ss << i + 1 << ") ID Prodotto: " << risultato.second[i].ID
               << " Nome: " << risultato.second[i].nome
               << " Descrizione: " << risultato.second[i].descrizione
               << " Fornitore: " << risultato.second[i].fornitore
               << " Prezzo: " << risultato.second[i].prezzo << "\n";
        }

        response.send(Pistache::Http::Code::Ok, ss.str());
    } else if (risultato.first == 0) {
        response.send(Pistache::Http::Code::Ok, "Nessun prodotto disponibile");
    } else {
        response.send(Pistache::Http::Code::Internal_Server_Error, "Errore nel recupero dei prodotti");
    }

    // Pulisci la memoria allocata dinamicamente per i prodotti
    if (risultato.second != nullptr) {
        delete[] risultato.second;
    }
}

