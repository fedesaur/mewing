#include "routes.h"
#include <pistache/http.h>
#include <pistache/endpoint.h>
#include <pistache/router.h>
#include <iostream>

void defineRoutes(Pistache::Rest::Router& router) {
    // Registrazione delle rotte con funzioni globali
    Pistache::Rest::Routes::Post(router, "/autentica", Pistache::Rest::Routes::bind(&authenticateUser));
    Pistache::Rest::Routes::Post(router, "/modificaNome", Pistache::Rest::Routes::bind(&modificaNomeHttp));
}

void authenticateUser(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response) {
    // Simulazione del socket client, sostituibile con un sistema reale
    int clientSocket = 0; // In una reale applicazione, questo dovrebbe essere l'ID del socket

    // Recupera i dati dalla richiesta HTTP (ad esempio il body contiene email)
    auto body = request.body();
    if (body.empty()) {
        response.send(Pistache::Http::Code::Bad_Request, "Missing authentication data");
        return;
    }

    // Qui ci aspettiamo un JSON con l'email
    std::string email;
    try {
        auto json = nlohmann::json::parse(body); // Usa una libreria per gestire il parsing JSON
        email = json.at("email").get<std::string>();
    } catch (const std::exception& e) {
        response.send(Pistache::Http::Code::Bad_Request, "Invalid JSON format");
        return;
    }

    // Simula la chiamata alla funzione di autenticazione passando il clientSocket
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

