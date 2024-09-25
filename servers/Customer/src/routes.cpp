#include "routes.h"
#include "../metodi/autenticazione.h"
#include "../metodi/recuperaProdotti.h"
#include "../metodi/recuperaCarrello.h"
#include "../metodi/gestisciCarrello.h"
#include <pistache/http.h>
#include <pistache/endpoint.h>
#include <pistache/router.h>
#include <iostream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

void defineRoutes(Pistache::Rest::Router& router) {
    // Registrazione delle rotte con funzioni globali
    Pistache::Rest::Routes::Get(router, "/autentica/:email", Pistache::Rest::Routes::bind(&autenticaCustomer));
    Pistache::Rest::Routes::Put(router, "/autentica/", Pistache::Rest::Routes::bind(&creaCustomer));
    Pistache::Rest::Routes::Post(router, "/:email/", Pistache::Rest::Routes::bind(&modificaInfo));
    Pistache::Rest::Routes::Get(router, "/:email/indirizzi/", Pistache::Rest::Routes::bind(&getIndirizzi));
    Pistache::Rest::Routes::Get(router, "/prodotti", Pistache::Rest::Routes::bind(&getProdotti));
    Pistache::Rest::Routes::Post(router, "/addToCarrello/:email/:prodotto/:quantita", Pistache::Rest::Routes::bind(&addProdottoToCarrello));
    Pistache::Rest::Routes::Get(router, "/:email/carrello/", Pistache::Rest::Routes::bind(&getCarrello));
    Pistache::Rest::Routes::Post(router, "/ordina/:email", Pistache::Rest::Routes::bind(&ordina));

}

//curl -X GET http://localhost:5001/autentica/abc@abc.it
void autenticaCustomer(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response)
{
    // Recupera l'email dal percorso
    std::string email = request.param(":email").as<std::string>();

    if (email.empty()) {
        response.send(Pistache::Http::Code::Bad_Request, "Email not provided\n");
        return;
    }

    // Ora chiama la funzione autentica
    int ID = autentica(email.c_str());

    if (ID > 0) {
        response.send(Pistache::Http::Code::Ok, "Customer authenticated\n");
    } else if (ID == 0) {
        response.send(Pistache::Http::Code::Not_Found, "User isn't in the system. You have to create it first\n");
    } else {
        response.send(Pistache::Http::Code::Unauthorized, "Authentication failed\n");
    }
}

//curl -X PUT -H "Content-Type: application/json" -d '{"nome": "Fabrizio", "cognome": "Fibroni", "email" : "fabri@email.com", "via" : "Via lotteria", "civico": 12, "cap" : "12345", "city" : "Palermo", "stato" : "Repubblica delle Banane"}' http://localhost:5001/autentica/
void creaCustomer(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response)
{
    json dati = json::parse(request.body());
    
    // Controlla se i dati forniti dall'utente sono presenti, non null e corretti
    if (!dati.contains("email") || dati["email"].is_null() || dati["email"].get<std::string>().empty()) 
        return response.send(Pistache::Http::Code::Bad_Request, "Email not provided\n");
    
    if (!dati.contains("nome") || dati["nome"].is_null() || dati["nome"].get<std::string>().empty()) 
        return response.send(Pistache::Http::Code::Bad_Request, "Name not provided\n");
    
    if (!dati.contains("cognome") || dati["cognome"].is_null() || dati["cognome"].get<std::string>().empty()) 
        return response.send(Pistache::Http::Code::Bad_Request, "Surname not provided\n");
    
    if (!dati.contains("via") || dati["via"].is_null() || dati["via"].get<std::string>().empty()) 
        return response.send(Pistache::Http::Code::Bad_Request, "Via not provided\n");
    
    if (!dati.contains("civico") || dati["civico"].is_null() || !dati["civico"].is_number()) 
        return response.send(Pistache::Http::Code::Bad_Request, "Civico not provided or not a number\n");
    
    if (!dati.contains("cap") || dati["cap"].is_null() || dati["cap"].get<std::string>().empty()) 
        return response.send(Pistache::Http::Code::Bad_Request, "CAP not provided\n");
    
    if (!dati.contains("city") || dati["city"].is_null() || dati["city"].get<std::string>().empty()) 
        return response.send(Pistache::Http::Code::Bad_Request, "City not provided\n");
    
    if (!dati.contains("stato") || dati["stato"].is_null() || dati["stato"].get<std::string>().empty()) 
        return response.send(Pistache::Http::Code::Bad_Request, "State not provided\n");

    // Recupera i dati convertiti
    std::string email = dati["email"].get<std::string>();
    std::string nome = dati["nome"].get<std::string>();
    std::string cognome = dati["cognome"].get<std::string>();
    std::string via = dati["via"].get<std::string>();
    int civico = dati["civico"].get<int>();
    std::string CAP = dati["cap"].get<std::string>();
    std::string city = dati["city"].get<std::string>();
    std::string stato = dati["stato"].get<std::string>();

    // Verifica la lunghezza dei campi
    if (email.length() > 50) 
        return response.send(Pistache::Http::Code::Bad_Request, "Mail length is above 50 characters\n");
    
    if (nome.length() > 20) 
        return response.send(Pistache::Http::Code::Bad_Request, "Name length is above 20 characters\n");
    
    if (cognome.length() > 20) 
        return response.send(Pistache::Http::Code::Bad_Request, "Surname length is above 20 characters\n");
    
    if (via.length() > 30) 
        return response.send(Pistache::Http::Code::Bad_Request, "Via length is above 30 characters\n");
    
    if (CAP.length() != 5 || !isNumber(CAP)) 
        return response.send(Pistache::Http::Code::Bad_Request, "CAP must be a string of 5 numbers\n");
    
    if (city.length() > 30) 
        return response.send(Pistache::Http::Code::Bad_Request, "City length is above 30 characters\n");
    
    if (stato.length() > 50) 
        return response.send(Pistache::Http::Code::Bad_Request, "State length is above 50 characters\n");
    
    // Chiama la funzione per creare il nuovo Fornitore
    bool esito = creaFornitore(email.c_str(), nome.c_str(), cognome.c_str(), via.c_str(), civico, CAP.c_str(), city.c_str(), stato.c_str());
    
    if (esito) {
        response.send(Pistache::Http::Code::Created, "Customer created\n");
    } else {
        response.send(Pistache::Http::Code::Unauthorized, "Failed to create the Customer\n");
    }
}


//curl -X POST -H "Content-Type: application/json" -d '{"nome": "Fabrizione", "cognome": "Napoli"}' http://localhost:5002/abc@abc.it/
void modificaInfo(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response)
{
    // Recupera l'email del fornitore tra i parametri
    std::string email = request.param(":email").as<std::string>();
    json dati = json::parse(request.body());
    // Controlla se i dati forniti dall'utente sono presenti e corretti
    if (!dati.contains("nome") || dati["nome"].empty()) response.send(Pistache::Http::Code::Bad_Request, "Name not provided\n");
    if (!dati.contains("cognome") || dati["cognome"].empty()) response.send(Pistache::Http::Code::Bad_Request, "Surname not provided\n");
    
    std::string nome = dati["nome"];
    std::string cognome = dati["cognome"];
    if (nome.length() > 20) response.send(Pistache::Http::Code::Bad_Request, "Name length is above 20 characters\n");
    if (cognome.length() > 20) response.send(Pistache::Http::Code::Bad_Request, "Surnam length is above 20 characters\n");

    bool esito = modificaInfoCustomer(email.c_str(), nome.c_str(), cognome.c_str());
    if (esito) {
        response.send(Pistache::Http::Code::Created, "Info changed\n");
    } else {
        response.send(Pistache::Http::Code::Unauthorized, "Failed to change your info\n");
    }
}

//curl -X GET http://localhost:5001/abc@abc.it/indirizzi/
void getIndirizzi(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response)
{
    std::stringstream ss;
    redisContext *c2r; // c2r contiene le info sul contesto
	redisReply *reply; // reply contiene le risposte da Redis
    redisReply *addressReply;
    Indirizzo* INDIRIZZI;
    int RIGHE;

    std::string email = request.param(":email").as<std::string>();
    
    // Controlla che i parametri richiesti siano stati forniti
    if (email.empty()) {
        response.send(Pistache::Http::Code::Bad_Request, "Email not provided\n");
        return;
    }

    // Connessione a Redis
    c2r = redisConnect(REDIS_IP, REDIS_PORT); // Redis su localhost
    if (c2r == nullptr || c2r->err) {
        std::cerr << "Errore nella connessione a Redis" << std::endl;
        response.send(Pistache::Http::Code::Internal_Server_Error, "Failed to connect to Redis");
        return;
    }

    // Recupera la lista di ID prodotti per l'email dal Redis
    reply = RedisCommand(c2r, "LRANGE indirizzi:%s 0 -1", email.c_str());
    if (reply->type == REDIS_REPLY_ARRAY && reply->elements > 0) // Recupera gli indirizzi da Redis
    {
        RIGHE = reply->elements;
        INDIRIZZI = new Indirizzo[RIGHE];  // Array dinamico di prodotti
        
        for (int i = 0; i < RIGHE; i++) 
        {
            std::string indirizzoID = reply->element[i]->str;

            // Debug: Stampa l'ID del prodotto che stai per recuperare
            std::cout << "Recuperando indirizzo con ID: " << indirizzoID << std::endl;
            INDIRIZZI[i].ID = std::stoi(indirizzoID);

            // Recupera il prodotto come hash da Redis
            addressReply = RedisCommand(c2r, "HGETALL indirizzo:%s", indirizzoID.c_str());
    
            // Verifica il risultato del recupero...
            if ( addressReply->type == REDIS_REPLY_ARRAY && addressReply->elements == 10) // 5 dati Richiesti: Via, Civico, CAP, Città, Stato
            { //... e asssocia i valori dell'indirizzo recuperato dallo stream Redis ad un oggetto Indirizzo 
        
                INDIRIZZI[i].via = (addressReply->element[1]->str);
                INDIRIZZI[i].civico = std::atoi(addressReply->element[3]->str);
                INDIRIZZI[i].CAP = (addressReply->element[5]->str);
                INDIRIZZI[i].citta = (addressReply->element[7]->str);
                INDIRIZZI[i].stato = (addressReply->element[9]->str);
            } else {
                std::cerr << "Errore nel recupero di un indirizzo da Redis" << std::endl;
                freeReplyObject(addressReply);
                redisFree(c2r);
                return;
            }
        }
        // Stampa i prodotti
        ss << "\nINDIRIZZI REGISTRATI:\n";
        for (int i = 0; i < RIGHE; i++) 
        {
            ss << i + 1 << ") ID Indirizzo: " << INDIRIZZI[i].ID
               << " Via: " << INDIRIZZI[i].via
               << " Civico: " << INDIRIZZI[i].civico
               << " CAP: " << INDIRIZZI[i].CAP 
               << " Città: " << INDIRIZZI[i].citta
               << " Stato: " << INDIRIZZI[i].stato <<"\n";
        }

        // Invia la risposta con i prodotti
        response.send(Pistache::Http::Code::Ok, ss.str());
        delete[] INDIRIZZI; // Libera la memoria allocata dinamicamente
    } else {
        // Nessun prodotto trovato in Redis
        response.send(Pistache::Http::Code::Ok, "Nessun prodotto disponibile in Redis");
    }
    freeReplyObject(reply);
    redisFree(c2r);  // Chiudi la connessione a Redis
    return;
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

int recuperaCustomerID(std::string email)
{
    int ID;
    PGresult *res;
    char comando[1000];
    Con2DB db(HOSTNAME, DB_PORT, USERNAME, PASSWORD, DB_NAME); // Effettua la connessione al database

    // Prepara la query per cercare l'ID cliente tramite l'email
    sprintf(comando, "SELECT id FROM customers WHERE mail = '%s' ", email.c_str());
    
    try
    {
        res = db.ExecSQLtuples(comando);
    	int rows = PQntuples(res);
		ID = 0;
    	if (rows > 0) // Se viene trovato un utente con quella mail...
    	{
			//...vengono recuperati i suoi dati ed inviati al server tramite Redis
        	ID = atoi(PQgetvalue(res, 0, PQfnumber(res, "id")));
    	}
    }
    catch(...)
    {
        ID = -1;
    }
    PQclear(res);
    return ID;
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
    std::pair<int, Prodotto*> risultato = recuperaCarrello(clientSocket);
    
    
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
        for (int i = 0; i < risultato.first; ++i) {
            ss << i + 1 << ") ID Prodotto: " << risultato.second[i].ID
               << " NomeP: " << risultato.second[i].nome
               << " Quantita: " << risultato.second[i].quantita
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
    
    response.send(Pistache::Http::Code::Ok, "carrello visualizzato");
    
    

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
    /*
    redisContext *c2r = redisConnect(REDIS_IP, REDIS_PORT);
    if (c2r == nullptr || c2r->err) {
        response.send(Pistache::Http::Code::Internal_Server_Error, "Unable to connect to Redis");
        return;
    }
    */
    
    // Prepara il comando per aggiungere l'email allo stream
    //redisReply* reply = static_cast<redisReply*>(redisCommand(c2r, "XADD %s * id %s", READ_STREAM, customerID));

    // Controlla l'esito del comando Redis
    /*
    if (reply == nullptr || reply->type != REDIS_REPLY_STRING) {
        response.send(Pistache::Http::Code::Internal_Server_Error, "Error writing email to Redis stream");
        redisFree(c2r);
        return;
    }
    */

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
