#include "routes.h"
#include "../metodi/autenticazione.h"
#include <pistache/http.h>
#include <pistache/endpoint.h>
#include <pistache/router.h>
#include <iostream>

void defineRoutes(Pistache::Rest::Router& router) {
    // Registrazione delle rotte con funzioni globali
    Pistache::Rest::Routes::Post(router, "/autentica/:email", Pistache::Rest::Routes::bind(&authenticateUser));
    Pistache::Rest::Routes::Post(router, "/modificaNome", Pistache::Rest::Routes::bind(&modificaNomeHttp));
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



// Funzione unica per gestire la modifica del nome
void modificaNomeHttp(const Pistache::Rest::Request& req, Pistache::Http::ResponseWriter response) {
    std::string email, nome;
    redisContext *c2r;
    redisReply *reply;

    // Leggi i parametri dalla query string
    auto emailOpt = req.query().get("email");
    auto nomeOpt = req.query().get("nome");

    // Verifica se i parametri sono stati forniti
    if (!emailOpt || !nomeOpt) {
        response.send(Pistache::Http::Code::Bad_Request, "Parametri email o nome mancanti");
        return;
    }

    email = emailOpt.value();
    nome = nomeOpt.value();

    // Valida il nome
    if (nome.length() > 20 || nome.length() == 0) {
        response.send(Pistache::Http::Code::Bad_Request, "Il nome deve avere massimo 20 caratteri e minimo 1");
        return;
    }

    // Connessione a Redis
    c2r = redisConnect(REDIS_IP, REDIS_PORT);
    if (c2r == nullptr || c2r->err) {
        response.send(Pistache::Http::Code::Internal_Server_Error, "Errore nella connessione a Redis");
        return;
    }

    // Scrivi nello stream Redis
    reply = (redisReply*)redisCommand(c2r, "XADD %s * email %s nome %s", 
                                      WRITE_STREAM, email.c_str(), nome.c_str());
    if (reply == nullptr) {
        response.send(Pistache::Http::Code::Internal_Server_Error, "Errore nel comando Redis");
        redisFree(c2r);
        return;
    }

    // Liberiamo la memoria del comando Redis
    freeReplyObject(reply);
    redisFree(c2r);

    // Rispondi al client
    response.send(Pistache::Http::Code::Ok, "Dati inviati per la modifica");
}
