#include "routes.h"
#include <pistache/http.h>
#include <pistache/endpoint.h>
#include <pistache/router.h>
#include <iostream>

void defineRoutes(Pistache::Rest::Router& router) {
    // Registrazione delle rotte con funzioni globali
    Pistache::Rest::Routes::Post(router, "/autentica", authenticateUser);
    Pistache::Rest::Routes::Post(router, "/modificaNome", modificaNomeHttp);
}

void authenticateUser(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response) {
    // Implementa l'autenticazione utente qui
    response.send(Pistache::Http::Code::Ok, "User authenticated");
}

void modificaNomeHttp(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response) {
    // Implementa la logica di modifica del nome qui
    response.send(Pistache::Http::Code::Ok, "Name updated");
}
