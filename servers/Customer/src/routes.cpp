#include "routes.h"
#include <pistache/http.h>
#include <pistache/endpoint.h>
#include <pistache/router.h>
#include <iostream>

void defineRoutes(Pistache::Rest::Router& router) {
    // Rotta per l'autenticazione
    router.addRoute(
        Pistache::Rest::Routes::Post("/autentica", authenticateUser)
    );

    // Rotta per la modifica del nome
    router.addRoute(
        Pistache::Rest::Routes::Post("/modificaNome", modificaNomeHttp)
    );
}

void authenticateUser(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response) {
    // Implementa l'autenticazione utente qui
    // Assumi che la richiesta contenga le informazioni necessarie
    response.send(Pistache::Http::Code::Ok, "User authenticated");
}

void modificaNomeHttp(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response) {
    // Implementa la logica di modifica del nome qui
    // Assumi che la richiesta contenga le informazioni necessarie
    response.send(Pistache::Http::Code::Ok, "Name updated");
}

