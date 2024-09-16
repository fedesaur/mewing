#include "routes.h"
void defineRoutes(crow::SimpleApp& app) {
    // Rotta per l'autenticazione
    CROW_ROUTE(app, "/autentica").methods("POST"_method)
    ([](const crow::request& req) {
        return authenticateUser(req);
    });

    CROW_ROUTE(app, "/modificaNome").methods("POST"_method)
    ([](const crow::request& req, crow::response& res) {
        return modificaNomeHttp(req, res);
    });
}
