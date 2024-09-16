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

    // Rotta per aggiungere prodotti al carrello
    CROW_ROUTE(app, "/carrello/add").methods("POST"_method)
    ([](const crow::request& req) {
        return addToCart(req);
    });

    // Rotta per rimuovere prodotti dal carrello
    CROW_ROUTE(app, "/carrello/remove").methods("POST"_method)
    ([](const crow::request& req) {
        return removeFromCart(req);
    });

    // Rotta per gestire i metodi di pagamento
    CROW_ROUTE(app, "/pagamenti").methods("POST"_method)
    ([](const crow::request& req) {
        return managePayment(req);
    });
}
