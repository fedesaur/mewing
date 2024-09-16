#include "crow.h"
#include "customer.h"  // Include la logica del cliente
#include "cart.h"      // Include la logica del carrello
#include "payment.h"   // Include la logica dei pagamenti

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
