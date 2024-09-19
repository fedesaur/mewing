#include "routes.h"
#include "../metodi/autenticazione.h"
#include "../metodi/recuperaProdotti.h"
#include "../metodi/recuperaCarrello.h"
#include "../metodi/gestisciCarrello.h"
#include <pistache/http.h>
#include <pistache/endpoint.h>
#include <pistache/router.h>
#include <iostream>

void defineRoutes(Pistache::Rest::Router& router) {
    // Registrazione delle rotte con funzioni globali
    Pistache::Rest::Routes::Post(router, "/autentica/:email", Pistache::Rest::Routes::bind(&authenticateUser));
    Pistache::Rest::Routes::Post(router, "/modificaNome", Pistache::Rest::Routes::bind(&modificaNomeHttp));
    Pistache::Rest::Routes::Get(router, "/prodotti", Pistache::Rest::Routes::bind(&getProdotti));
    Pistache::Rest::Routes::Post(router, "/addToCarrello/:email/:prodotto/:quantita", Pistache::Rest::Routes::bind(&addProdottoToCarrello));
    Pistache::Rest::Routes::Get(router, "/carrello/:email", Pistache::Rest::Routes::bind(&getCarrello));
    Pistache::Rest::Routes::Post(router, "/ordina/:email", Pistache::Rest::Routes::bind(&ordina));

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

void addProdottoToCarrello(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response) {
    // Recupera i parametri dalla richiesta
    auto email = request.param(":email").as<std::string>();
    auto prodottoID = request.param(":prodotto").as<int>();  // Supponendo che l'ID del prodotto sia un intero
    auto quantita = request.param(":quantita").as<int>();    // Quantità come intero

    // Recupera l'ID del cliente basato sull'email
    int customerID = recuperaCustomerID(email);
    if (customerID <= 0) {
        response.send(Pistache::Http::Code::Internal_Server_Error, "Errore nel recupero dell'ID cliente");
        return;
    }

    // Simula un clientSocket 
    int clientSocket = 0;

    // Recupera i prodotti (o il prodotto specifico)
    //std::pair<int, Prodotto*> risultatoProdotti = recuperaProdotti(clientSocket);
    //int numeroProdotti = risultatoProdotti.first;
    //Prodotto* prodotti = risultatoProdotti.second;

    // Aggiungi il prodotto al carrello
    bool esito = aggiungiCarrelloDB(prodottoID, customerID, quantita);

    if (esito) {
        response.send(Pistache::Http::Code::Ok, "Prodotto aggiunto al carrello con successo");
    } else {
        response.send(Pistache::Http::Code::Internal_Server_Error, "Errore durante l'aggiunta del prodotto al carrello");
    }

    // Libera la memoria dei prodotti recuperati, se necessario
    //delete[] prodotti;
}

int recuperaCustomerID(const std::string& email) {
    PGconn *conn = PQconnectdb("host=localhost port=5432 dbname=mewingdb user=admin password=admin");

    if (PQstatus(conn) != CONNECTION_OK) {
        std::cerr << "Errore di connessione al database: " << PQerrorMessage(conn) << std::endl;
        PQfinish(conn);
        return -1; // Restituisci un ID non valido in caso di errore
    }

    // Prepara la query per cercare l'ID cliente tramite l'email
    const char *query = "SELECT id FROM customers WHERE mail = $1";
    const char *paramValues[1] = { email.c_str() };

    PGresult *res = PQexecParams(conn, query, 1, NULL, paramValues, NULL, NULL, 0);

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        std::cerr << "Errore nell'esecuzione della query: " << PQerrorMessage(conn) << std::endl;
        PQclear(res);
        PQfinish(conn);
        return -1;
    }
    
    std::cout << "sono qui" << std::endl;
    // Controlla se è stato trovato un cliente
    int customerID = -1;
    if (PQntuples(res) == 1) {
        customerID = atoi(PQgetvalue(res, 0, 0)); // Restituisci l'ID cliente
    } else {
        std::cerr << "Cliente non trovato o più di un risultato" << std::endl;
    }

    // Libera la memoria e chiudi la connessione
    PQclear(res);
    PQfinish(conn);

    return customerID;
}


void getCarrello(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response) {
    // Recupera l'email dai parametri della richiesta HTTP
    auto email = request.param(":email").as<std::string>();  
    
    /*
    redisContext *c2r = redisConnect(REDIS_IP, REDIS_PORT);
    if (c2r == nullptr || c2r->err) {
        response.send(Pistache::Http::Code::Internal_Server_Error, "Unable to connect to Redis");
        return;
    }

    int customerID = recuperaCustomerID(email);
    if (customerID <= 0) {
        response.send(Pistache::Http::Code::Internal_Server_Error, "Errore nel recupero dell'ID cliente");
        return;
    }

    // Prepara il comando per aggiungere l'email allo stream
    redisReply* reply = static_cast<redisReply*>(redisCommand(c2r, "XADD %s * id %s", READ_STREAM, customerID));

    // Controlla l'esito del comando Redis
    if (reply == nullptr || reply->type != REDIS_REPLY_STRING) {
        response.send(Pistache::Http::Code::Internal_Server_Error, "Error writing email to Redis stream");
        redisFree(c2r);
        return;
    }
    
    */

    
    int clientSocket = 0;  // In questa applicazione, simuliamo la logica del socket del client.

    // Recupera il carrello utilizzando l'email
    std::pair<int, Prodotto*> risultato = getCarrellos(clientSocket);
    
    
    // Verifica se il carrello è stato trovato
    if (risultato.first == -1) {
        response.send(Pistache::Http::Code::Not_Found, "Carrello non trovato per questo utente.");
        return;
    }

    // Numero di righe/prodotti nel carrello
    int righeCarrello = risultato.first;
    Prodotto* carrello = risultato.second;
    
    // Costruisci la risposta
    if (risultato.first > 0 && risultato.second != nullptr) {
        std::stringstream ss;
        ss << "PRODOTTI NEL CARRELLO:\n";

        // Itera sui prodotti e li inserisce nella stringa di risposta
        for (int i = 0; i < righeCarrello; i++)
        {
            // Recupera gli attributi dei prodotti dal carrello...
            int ID = carrello[i].ID;
            const char* descrizione = carrello[i].descrizione;
            double prezzo = carrello[i].prezzo;
            const char* nomeP = carrello[i].nome;
            const char* fornitore = carrello[i].fornitore;
            int quantita = carrello[i].quantita;
            // ...e li invia all'utente così che possa visualizzarli ed effettuarci operazioni
            std::string prodotto = std::to_string(i+1) + ") ID Prodotto: " + std::to_string(ID) +
             " Nome Prodotto: " + nomeP + 
             " Descrizione: " + descrizione + 
             " Fornitore: " + fornitore + 
             " Prezzo Prodotto: " + std::to_string(prezzo) + 
             " Quantità :" + std::to_string(quantita) + "\n";
	        send(clientSocket, prodotto.c_str(), prodotto.length(), 0);
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
    
    //mostraCarrello(clientSocket, carrello, righeCarrello);
    
    //response.send(Pistache::Http::Code::Ok, "carrello visualizzato");

    // Libera la memoria del carrello
    //delete[] carrello;
    
    
    std::cout << "quii" <<std::endl;
}


void ordina(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response) {
    int righe;
    Prodotto* prodotti;
    // Recupera i parametri dalla richiesta
    auto email = request.param(":email").as<std::string>();

    // Recupera l'ID del cliente basato sull'email
    int customerID = recuperaCustomerID(email);
    if (customerID <= 0) {
        response.send(Pistache::Http::Code::Internal_Server_Error, "Errore nel recupero dell'ID cliente");
        return;
    }
    
    redisContext *c2r = redisConnect(REDIS_IP, REDIS_PORT);
    if (c2r == nullptr || c2r->err) {
        response.send(Pistache::Http::Code::Internal_Server_Error, "Unable to connect to Redis");
        return;
    }

    // Prepara il comando per aggiungere l'email allo stream
    redisReply* reply = static_cast<redisReply*>(redisCommand(c2r, "XADD %s * id %s", READ_STREAM, customerID));

    // Controlla l'esito del comando Redis
    if (reply == nullptr || reply->type != REDIS_REPLY_STRING) {
        response.send(Pistache::Http::Code::Internal_Server_Error, "Error writing email to Redis stream");
        redisFree(c2r);
        return;
    }

    // Simula un clientSocket 
    int clientSocket = 0;
    std::pair<int, Prodotto*> risultatoCarrello = recuperaCarrello(clientSocket);
    risultatoCarrello.first = righe;
    risultatoCarrello.second = prodotti;
    bool esito=effettuaOrdine(clientSocket, customerID, righe, prodotti);
    if (esito) {
        response.send(Pistache::Http::Code::Ok, "Ordine fatto");
    } else {
        response.send(Pistache::Http::Code::Internal_Server_Error, "Errore durante l'ordine");
    }

    
    
}
