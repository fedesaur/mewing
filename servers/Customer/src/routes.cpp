#include "routes.h"
using json = nlohmann::json;

void defineRoutes(Pistache::Rest::Router& router) 
{
    // Registrazione delle rotte con funzioni globali
    Pistache::Rest::Routes::Get(router, "/autentica/:email", Pistache::Rest::Routes::bind(&autenticaCustomer));
    Pistache::Rest::Routes::Get(router, "/:email/indirizzi/", Pistache::Rest::Routes::bind(&getIndirizzi));
    Pistache::Rest::Routes::Get(router, "/:email/ordini/", Pistache::Rest::Routes::bind(&getOrdini));
    Pistache::Rest::Routes::Get(router, "/:email/carrello/", Pistache::Rest::Routes::bind(&getCarrello));
    Pistache::Rest::Routes::Get(router, "/:email/prodotti/", Pistache::Rest::Routes::bind(&getProdotti));
    Pistache::Rest::Routes::Put(router, "/:email/indirizzi/", Pistache::Rest::Routes::bind(&addIndirizzo));
    Pistache::Rest::Routes::Put(router, "/:email/ordini/", Pistache::Rest::Routes::bind(&ordina));
    Pistache::Rest::Routes::Put(router, "/:email/carrello/", Pistache::Rest::Routes::bind(&addProdottoToCarrello));
    Pistache::Rest::Routes::Put(router, "/autentica/", Pistache::Rest::Routes::bind(&creaCustomer));
    Pistache::Rest::Routes::Post(router, "/:email/", Pistache::Rest::Routes::bind(&modificaInfo));
    Pistache::Rest::Routes::Delete(router, "/:email/carrello/:productID", Pistache::Rest::Routes::bind(&removeCarrello));
    Pistache::Rest::Routes::Delete(router, "/:email/ordini/:ordineID", Pistache::Rest::Routes::bind(&annullaOrd));
    Pistache::Rest::Routes::Delete(router, "/:email/indirizzi/:addressID", Pistache::Rest::Routes::bind(&removeIndirizzo));
}

//curl -X GET http://localhost:5001/autentica/abc@abc.it
void autenticaCustomer(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response)
{
    std::string email = request.param(":email").as<std::string>(); // Recupera l'email dal percorso
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
        response.send(Pistache::Http::Code::Bad_Request, "Email not provided\n");
    
    if (!dati.contains("nome") || dati["nome"].is_null() || dati["nome"].get<std::string>().empty()) 
        response.send(Pistache::Http::Code::Bad_Request, "Name not provided\n");
    
    if (!dati.contains("cognome") || dati["cognome"].is_null() || dati["cognome"].get<std::string>().empty()) 
        response.send(Pistache::Http::Code::Bad_Request, "Surname not provided\n");
    
    if (!dati.contains("via") || dati["via"].is_null() || dati["via"].get<std::string>().empty()) 
        response.send(Pistache::Http::Code::Bad_Request, "Via not provided\n");
    
    if (!dati.contains("civico") || dati["civico"].is_null()) 
        response.send(Pistache::Http::Code::Bad_Request, "Civico not provided or not a number\n");
    
    if (!dati.contains("cap") || dati["cap"].is_null() || dati["cap"].get<std::string>().empty()) 
        response.send(Pistache::Http::Code::Bad_Request, "CAP not provided\n");
    
    if (!dati.contains("city") || dati["city"].is_null() || dati["city"].get<std::string>().empty()) 
        response.send(Pistache::Http::Code::Bad_Request, "City not provided\n");
    
    if (!dati.contains("stato") || dati["stato"].is_null() || dati["stato"].get<std::string>().empty()) 
        response.send(Pistache::Http::Code::Bad_Request, "State not provided\n");

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
        response.send(Pistache::Http::Code::Bad_Request, "Mail length is above 50 characters\n");
    
    if (nome.length() > 20) 
        response.send(Pistache::Http::Code::Bad_Request, "Name length is above 20 characters\n");
    
    if (cognome.length() > 20) 
        response.send(Pistache::Http::Code::Bad_Request, "Surname length is above 20 characters\n");
    
    if (via.length() > 30) 
        response.send(Pistache::Http::Code::Bad_Request, "Via length is above 30 characters\n");
    
    if (CAP.length() != 5 || !isNumber(CAP)) 
        response.send(Pistache::Http::Code::Bad_Request, "CAP must be a string of 5 numbers\n");
    
    if (city.length() > 30) 
        response.send(Pistache::Http::Code::Bad_Request, "City length is above 30 characters\n");
    
    if (stato.length() > 50) 
        response.send(Pistache::Http::Code::Bad_Request, "State length is above 50 characters\n");
    
    // Chiama la funzione per creare il nuovo Fornitore
    bool esito = crea(email.c_str(), nome.c_str(), cognome.c_str(), via.c_str(), civico, CAP.c_str(), city.c_str(), stato.c_str());
    
    if (esito) {
        response.send(Pistache::Http::Code::Created, "Customer created\n");
    } else {
        response.send(Pistache::Http::Code::Unauthorized, "Failed to create the Customer\n");
    }
}


//curl -X POST -H "Content-Type: application/json" -d '{"nome": "Fabrizione", "cognome": "Napoli"}' http://localhost:5001/abc@abc.it/
void modificaInfo(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response)
{
    int logID;
    // Recupera l'email del fornitore tra i parametri
    std::string email = request.param(":email").as<std::string>();
    json dati = json::parse(request.body());
    // Controlla se i dati forniti dall'utente sono presenti e corretti
    if (!dati.contains("nome") || dati["nome"].empty()) response.send(Pistache::Http::Code::Bad_Request, "Name not provided\n");
    if (!dati.contains("cognome") || dati["cognome"].empty()) response.send(Pistache::Http::Code::Bad_Request, "Surname not provided\n");
    
    std::string nome = dati["nome"];
    std::string cognome = dati["cognome"];
    if (nome.length() > 20) response.send(Pistache::Http::Code::Bad_Request, "Name length is above 20 characters\n");
    if (cognome.length() > 20) response.send(Pistache::Http::Code::Bad_Request, "Surname length is above 20 characters\n");
    
    int customerID = recuperaCustomerID(email);
    if (customerID <= 0) {
        response.send(Pistache::Http::Code::Internal_Server_Error, "Errore nel recupero dell'ID cliente\n");
        return;
    }

    logID = inserimentoOperazione(customerID, "Aggiornamento informazioni personali");
    if (logID == -1)
    {
        response.send(Pistache::Http::Code::Internal_Server_Error, "Errore nel sistema di monitoraggio\n");
        return;
    }

    bool esito = modificaInfoCustomer(email.c_str(), nome.c_str(), cognome.c_str());

    if (esito) {
        response.send(Pistache::Http::Code::Created, "Info changed\n");
        successoOperazione(logID);
    } else {
        response.send(Pistache::Http::Code::Unauthorized, "Failed to change your info\n");
        fallimentoOperazione(logID);
    }
    return;
}

//curl -X GET http://localhost:5001/abc@abc.it/indirizzi/
void getIndirizzi(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response)
{
    int logID;
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
    // Recupera l'ID del cliente basato sull'email
    int customerID = recuperaCustomerID(email);
    if (customerID <= 0) {
        response.send(Pistache::Http::Code::Internal_Server_Error, "Errore nel recupero dell'ID cliente\n");
        return;
    }

    logID = inserimentoOperazione(customerID, "Recupero degli Indirizzi registrati");
    if (logID == -1)
    {
        response.send(Pistache::Http::Code::Internal_Server_Error, "Errore nel sistema di monitoraggio\n");
        return;
    }

    bool pubblicati = recuperaIndirizzi(email.c_str(), customerID); // Immette i prodotti presenti nel carrello nello stream
    if (!pubblicati) 
    {
        response.send(Pistache::Http::Code::Internal_Server_Error, "Failed to recover addresses' info\n");
        fallimentoOperazione(logID);
        return;
    }

    // Connessione a Redis
    c2r = redisConnect(REDIS_IP, REDIS_PORT); // Redis su localhost
    if (c2r == nullptr || c2r->err) {
        std::cerr << "Errore nella connessione a Redis" << std::endl;
        response.send(Pistache::Http::Code::Internal_Server_Error, "Failed to connect to Redis");
        fallimentoOperazione(logID);
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

            // Recupera il prodotto come hash da Redis
            addressReply = RedisCommand(c2r, "HGETALL indirizzo:%s", indirizzoID.c_str());
    
            // Verifica il risultato del recupero...
            if ( addressReply->type == REDIS_REPLY_ARRAY && addressReply->elements == 12) // 5 dati Richiesti: Via, Civico, CAP, Città, Stato
            { //... e asssocia i valori dell'indirizzo recuperato dallo stream Redis ad un oggetto Indirizzo 
            
                INDIRIZZI[i].ID = std::atoi(addressReply->element[1]->str);        
                INDIRIZZI[i].via = (addressReply->element[3]->str);
                INDIRIZZI[i].civico = std::atoi(addressReply->element[5]->str);
                INDIRIZZI[i].CAP = (addressReply->element[7]->str);
                INDIRIZZI[i].citta = (addressReply->element[9]->str);
                INDIRIZZI[i].stato = (addressReply->element[11]->str);
            } else {
                std::cerr << "Errore nel recupero di un indirizzo da Redis" << std::endl;
                fallimentoOperazione(logID);
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
    successoOperazione(logID);
    return;
}

//curl -X GET http://localhost:5001/abc@abc.it/ordini/
void getOrdini(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response)
{
    int logID;
    std::stringstream ss;
    redisContext *c2r; // c2r contiene le info sul contesto
    redisReply *reply; // reply contiene le risposte da Redis
    redisReply *orderReply;
    Ordine* ORDINI;
    int RIGHE;

    std::string email = request.param(":email").as<std::string>();
    
    // Controlla che i parametri richiesti siano stati forniti
    if (email.empty()) {
        response.send(Pistache::Http::Code::Bad_Request, "Email not provided\n");
        return;
    }
    // Recupera l'ID del cliente basato sull'email
    int customerID = recuperaCustomerID(email);
    if (customerID <= 0) {
        response.send(Pistache::Http::Code::Internal_Server_Error, "Errore nel recupero dell'ID cliente\n");
        return;
    }

    logID = inserimentoOperazione(customerID, "Recupero degli Ordini effettuati");
    if (logID == -1)
    {
        response.send(Pistache::Http::Code::Internal_Server_Error, "Errore nel sistema di monitoraggio\n");
        return;
    }
    
    bool pubblicati = recuperaOrdini(email.c_str()); // Immette i prodotti presenti nel carrello nello stream
    if (!pubblicati) 
    {
        response.send(Pistache::Http::Code::Internal_Server_Error, "Failed to recover orders' info\n");
        fallimentoOperazione(logID);
        return;
    }

    // Connessione a Redis
    c2r = redisConnect(REDIS_IP, REDIS_PORT); // Redis su localhost
    if (c2r == nullptr || c2r->err) {
        std::cerr << "Errore nella connessione a Redis" << std::endl;
        response.send(Pistache::Http::Code::Internal_Server_Error, "Failed to connect to Redis");
        fallimentoOperazione(logID);
        return;
    }

    // Recupera la lista di ID prodotti per l'email dal Redis
    reply = RedisCommand(c2r, "LRANGE ordini:%s 0 -1", email.c_str());
    if (reply->type == REDIS_REPLY_ARRAY && reply->elements > 0) // Recupera gli indirizzi da Redis
    {
        RIGHE = reply->elements;
        ORDINI = new Ordine[RIGHE];  // Array dinamico di prodotti
        
        for (int i = 0; i < RIGHE; i++) 
        {
            std::string productID = reply->element[i]->str;

            // Recupera il prodotto come hash da Redis
            orderReply = RedisCommand(c2r, "HGETALL ordine:%s", productID.c_str());
    
            // Verifica il risultato del recupero...
            if (orderReply->type == REDIS_REPLY_ARRAY && orderReply->elements == 14) // 7 dati Richiesti: ID Ordine, Stato, Data Richiesta, Data Consegna, Totale, Tipo di Pagamento, Indirizzo
            { //... e asssocia i valori dell'indirizzo recuperato dallo stream Redis ad un oggetto Indirizzo 
            
                ORDINI[i].ID = std::atoi(orderReply->element[1]->str);
                ORDINI[i].Stato = (orderReply->element[3]->str);
                ORDINI[i].DataRichiesta = (orderReply->element[5]->str);
                ORDINI[i].DataConsegna = (orderReply->element[7]->str);
                ORDINI[i].Totale = std::atof(orderReply->element[9]->str);
                ORDINI[i].Pagamento = (orderReply->element[11]->str);
                ORDINI[i].Indirizzo = std::atoi(orderReply->element[13]->str);
            } else {
                std::cerr << "Errore nel recupero di un ordine da Redis" << std::endl;
                fallimentoOperazione(logID);
                freeReplyObject(orderReply);
                redisFree(c2r);
                return;
            }
        }
        // Stampa i prodotti
        ss << "\nORDINI REGISTRATI:\n";
        for (int i = 0; i < RIGHE; i++) 
        {
            ss << i+1 << ") ID Ordine: " << ORDINI[i].ID
                << " Data Richiesta: " << ORDINI[i].DataRichiesta
                << " Data Consegna: (0 se non ancora effettuata) " << ORDINI[i].DataConsegna
                << " Stato Ordine: " << ORDINI[i].Stato
                << " Metodo Pagamento: " << ORDINI[i].Pagamento
                << " Totale Ordine: " << ORDINI[i].Totale
                << " ID Indirizzo: " << ORDINI[i].Indirizzo << "\n";
        }
        // Invia la risposta con i prodotti
        response.send(Pistache::Http::Code::Ok, ss.str());
        delete[] ORDINI; // Libera la memoria allocata dinamicamente
    } else {
        // Nessun prodotto trovato in Redis
        response.send(Pistache::Http::Code::Ok, "Nessun ordine registrato in Redis\n");
    }
    freeReplyObject(reply);
    redisFree(c2r);  // Chiudi la connessione a Redis
    successoOperazione(logID);
    return;
}

//curl -X GET http://localhost:5001/abc@abc.it/carrello/
void getCarrello(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response)
{
    int logID;
    std::stringstream ss;
    redisContext *c2r; // c2r contiene le info sul contesto
    redisReply *reply = nullptr; // reply contiene le risposte da Redis
    redisReply *productReply = nullptr;
    Prodotto* PRODOTTI = nullptr;
    int RIGHE;

    std::string email = request.param(":email").as<std::string>();

    // Controlla che i parametri richiesti siano stati forniti
    if (email.empty()) {
        response.send(Pistache::Http::Code::Bad_Request, "Email not provided\n");
        return;
    }
    
    int customerID = recuperaCustomerID(email);
    if (customerID <= 0) {
        response.send(Pistache::Http::Code::Internal_Server_Error, "Errore nel recupero dell'ID cliente\n");
        return;
    }

    logID = inserimentoOperazione(customerID, "Recupero del Carrello");
    if (logID == -1)
    {
        response.send(Pistache::Http::Code::Internal_Server_Error, "Errore nel sistema di monitoraggio\n");
        return;
    }
    
    // Recupera i prodotti del carrello, eventualmente recuperandoli dal DB se non presenti in Redis
    bool pubblicati = recuperaCarrello(customerID, email.c_str());
    if (!pubblicati) {
        fallimentoOperazione(logID);
        response.send(Pistache::Http::Code::Internal_Server_Error, "Failed to recover cart's info\n");
        return;
    }

    // Connessione a Redis
    c2r = redisConnect(REDIS_IP, REDIS_PORT); // Redis su localhost
    if (c2r == nullptr || c2r->err) {
        std::cerr << "Errore nella connessione a Redis: " << (c2r ? c2r->errstr : "null") << std::endl;
        response.send(Pistache::Http::Code::Internal_Server_Error, "Failed to connect to Redis");
        fallimentoOperazione(logID);
        return;
    }

    // Recupera la lista di ID prodotti per l'email da Redis
    reply = (redisReply*)redisCommand(c2r, "LRANGE carrello:%s 0 -1", email.c_str());
    if (reply == nullptr) {
        std::cerr << "Errore nel recupero della lista da Redis" << std::endl;
        redisFree(c2r);  // Chiudi connessione Redis
        response.send(Pistache::Http::Code::Internal_Server_Error, "Failed to recover cart from Redis");
        fallimentoOperazione(logID);
        return;
    }

    if (reply->type == REDIS_REPLY_ARRAY && reply->elements > 0) { // Recupera gli ID dei prodotti da Redis
        RIGHE = reply->elements;
        PRODOTTI = new Prodotto[RIGHE];  // Array dinamico di prodotti
        
        for (int i = 0; i < RIGHE; i++) {
            std::string productID = reply->element[i]->str;

            // Recupera il prodotto come hash da Redis
            productReply = (redisReply*)redisCommand(c2r, "HGETALL prodottoCarr:%s", productID.c_str());
            if (productReply == nullptr) {
                std::cerr << "Errore nel recupero del prodotto " << productID << " da Redis" << std::endl;
                delete[] PRODOTTI;
                freeReplyObject(reply);
                redisFree(c2r);
                fallimentoOperazione(logID);
                response.send(Pistache::Http::Code::Internal_Server_Error, "Failed to recover product from Redis");
                return;
            }

            // Verifica il risultato del recupero
            if (productReply->type == REDIS_REPLY_ARRAY && productReply->elements == 12) { 
                // Associa i valori del prodotto recuperato dallo stream Redis all'oggetto Prodotto
                PRODOTTI[i].ID = std::stoi(productReply->element[1]->str);
                PRODOTTI[i].descrizione = (productReply->element[3]->str);
                PRODOTTI[i].prezzo = std::atof(productReply->element[5]->str);
                PRODOTTI[i].nome = (productReply->element[7]->str);
                PRODOTTI[i].fornitore = (productReply->element[9]->str);
                PRODOTTI[i].quantita = std::stoi(productReply->element[11]->str);
            } else {
                std::cerr << "Errore nel recupero dei dettagli del prodotto da Redis" << std::endl;
                delete[] PRODOTTI;
                fallimentoOperazione(logID);
                freeReplyObject(productReply);
                freeReplyObject(reply);
                redisFree(c2r);
                response.send(Pistache::Http::Code::Internal_Server_Error, "Failed to parse product details from Redis");
                return;
            }
            //freeReplyObject(productReply); // Libera la memoria del productReply
        }

        // Stampa i prodotti
        ss << "\nPRODOTTI NEL CARRELLO:\n";
        for (int i = 0; i < RIGHE; i++) {
            ss << i + 1 << ") ID Prodotto: " << PRODOTTI[i].ID
               << " Nome Prodotto: " << PRODOTTI[i].nome
               << " Descrizione Prodotto: " << PRODOTTI[i].descrizione
               << " Prezzo: " << PRODOTTI[i].prezzo
               << " Quantità: " << PRODOTTI[i].quantita
               << " Nome Fornitore: " << PRODOTTI[i].fornitore << "\n";
        }

        // Invia la risposta con i prodotti
        response.send(Pistache::Http::Code::Ok, ss.str());
        delete[] PRODOTTI; // Libera la memoria allocata dinamicamente
    } else {
        // Nessun prodotto trovato in Redis
        response.send(Pistache::Http::Code::Ok, "Nessun prodotto nel carrello in Redis\n");
    }

    // Libera risorse
    freeReplyObject(reply);
    freeReplyObject(productReply);
    redisFree(c2r);  // Chiudi la connessione a Redis
    successoOperazione(logID);
    return;
}


//curl -X GET http://localhost:5001/abc@abc.it/prodotti/
void getProdotti(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response) 
{
    int logID;
    std::stringstream ss;
    redisContext *c2r; // c2r contiene le info sul contesto
    redisReply *reply; // reply contiene le risposte da Redis
    redisReply *productReply;
    Prodotto* PRODOTTI;
    int RIGHE;

    std::string email = request.param(":email").as<std::string>();
    
    // Controlla che i parametri richiesti siano stati forniti
    if (email.empty()) {
        response.send(Pistache::Http::Code::Bad_Request, "Email not provided\n");
        return;
    }

    int customerID = recuperaCustomerID(email);
    if (customerID <= 0) {
        response.send(Pistache::Http::Code::Internal_Server_Error, "Errore nel recupero dell'ID cliente\n");
        return;
    }

    logID = inserimentoOperazione(customerID, "Recupero dei Prodotti disponibili");
    if (logID == -1)
    {
        response.send(Pistache::Http::Code::Internal_Server_Error, "Errore nel sistema di monitoraggio\n");
        return;
    }
    
    bool pubblicati = recuperaProdotti(email.c_str()); // Immette i prodotti presenti nel carrello nello stream
    if (!pubblicati) 
    {
        response.send(Pistache::Http::Code::Internal_Server_Error, "Failed to recover products' info\n");
        fallimentoOperazione(logID);
        return;
    }

    // Connessione a Redis
    c2r = redisConnect(REDIS_IP, REDIS_PORT); // Redis su localhost
    if (c2r == nullptr || c2r->err) {
        std::cerr << "Errore nella connessione a Redis" << std::endl;
        response.send(Pistache::Http::Code::Internal_Server_Error, "Failed to connect to Redis");
        fallimentoOperazione(logID);
        return;
    }

    // Recupera la lista di ID prodotti per l'email dal Redis
    reply = RedisCommand(c2r, "LRANGE prodottiRic:%s 0 -1", email.c_str());
    if (reply->type == REDIS_REPLY_ARRAY && reply->elements > 0) // Recupera gli indirizzi da Redis
    {
        RIGHE = reply->elements;
        PRODOTTI = new Prodotto[RIGHE];  // Array dinamico di prodotti
        
        for (int i = 0; i < RIGHE; i++) 
        {
            std::string productID = reply->element[i]->str;

            // Recupera il prodotto come hash da Redis
            productReply = RedisCommand(c2r, "HGETALL prodottoRic:%s", productID.c_str());
    
            // Verifica il risultato del recupero...
            if (productReply->type == REDIS_REPLY_ARRAY && productReply->elements == 10) // 6 dati Richiesti: ID Indirizzo, Via, Civico, CAP, Città, Stato
            { //... e asssocia i valori dell'indirizzo recuperato dallo stream Redis ad un oggetto Indirizzo 
            
                PRODOTTI[i].ID = std::atoi(productReply->element[1]->str);
                PRODOTTI[i].descrizione = (productReply->element[3]->str);
                PRODOTTI[i].prezzo = std::atof(productReply->element[5]->str);
                PRODOTTI[i].nome = (productReply->element[7]->str);
                PRODOTTI[i].fornitore = (productReply->element[9]->str);
            } else {
                std::cerr << "Errore nel recupero di un indirizzo da Redis" << std::endl;
                fallimentoOperazione(logID);
                freeReplyObject(productReply);
                redisFree(c2r);
                return;
            }
        }
        // Stampa i prodotti
        ss << "\nPRODOTTI DISPONIBILI:\n";
        for (int i = 0; i < RIGHE; i++) 
        {
            ss << i + 1 << ") ID Prodotto: " << PRODOTTI[i].ID
               << " Nome Prodotto: " << PRODOTTI[i].nome
               << " Descrizione Prodotto: " << PRODOTTI[i].descrizione
               << " Prezzo: " << PRODOTTI[i].prezzo
               << " Nome Fornitore: " << PRODOTTI[i].fornitore <<"\n";
        }
        // Invia la risposta con i prodotti
        response.send(Pistache::Http::Code::Ok, ss.str());
        delete[] PRODOTTI; // Libera la memoria allocata dinamicamente
    } else {
        // Nessun prodotto trovato in Redis
        response.send(Pistache::Http::Code::Ok, "Nessun prodotto disponibile in Redis\n");
    }
    freeReplyObject(reply);
    freeReplyObject(productReply);
    redisFree(c2r);  // Chiudi la connessione a Redis
    successoOperazione(logID);
    return;
}

//curl -X PUT -H "Content-Type: application/json" -d '{"quantita": 10, "IDprodotto": 1}' http://localhost:5001/abc@abc.it/carrello/
void addProdottoToCarrello(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response) 
{
    int logID;
    char comando[100];
    // Recupera i parametri dalla richiesta
    std::string email = request.param(":email").as<std::string>();
    if (email.empty()) {
        response.send(Pistache::Http::Code::Bad_Request, "Email not provided\n");
        return;
    }
    
    json dati = json::parse(request.body());

    if (!dati.contains("quantita") || dati["quantita"].empty())
    {
        response.send(Pistache::Http::Code::Bad_Request, "Quantity not provided\n");
        return;
    }
    if (!dati.contains("IDprodotto") || dati["IDprodotto"].empty())
    {
        response.send(Pistache::Http::Code::Bad_Request, "ProductID not provided\n");
        return;
    }

    int prodottoID = dati["IDprodotto"];
    int quantita = dati["quantita"];
    if (prodottoID <= 0)
    {
        response.send(Pistache::Http::Code::Bad_Request, "ProductID must be a positive integer\n");
        return;
    }
    if (quantita <= 0)
    {
        response.send(Pistache::Http::Code::Bad_Request, "Quantity must be a positive integer\n");
        return;
    }

    // Recupera l'ID del cliente basato sull'email
    int customerID = recuperaCustomerID(email);
    if (customerID <= 0) {
        response.send(Pistache::Http::Code::Internal_Server_Error, "Errore nel recupero dell'ID cliente\n");
        return;
    }
    sprintf(comando, "Aggiunta del prodotto con ID = %d al carrello", prodottoID);
    logID = inserimentoOperazione(customerID, comando);
    if (logID == -1)
    {
        response.send(Pistache::Http::Code::Internal_Server_Error, "Errore nel sistema di monitoraggio\n");
        return;
    }

    bool esito = aggiungiCarrello(prodottoID, customerID, quantita);
    if (esito) {
        response.send(Pistache::Http::Code::Ok, "Prodotto aggiunto al carrello con successo\n");
        successoOperazione(logID);
    } else {
        response.send(Pistache::Http::Code::Internal_Server_Error, "Errore durante l'aggiunta del prodotto al carrello\n");
        fallimentoOperazione(logID);
    }
}

//curl -X PUT -H "Content-Type: application/json" -d '{"via" : "Via Salaria", "civico": 122, "cap" : "10123", "city" : "Roma", "stato" : "Puponia"}' http://localhost:5001/abc@abc.it/indirizzi/
void addIndirizzo(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response)
{
    int logID;
    std::string email = request.param(":email").as<std::string>();
    if (email.empty()) {
        response.send(Pistache::Http::Code::Bad_Request, "Email not provided\n");
        return;
    }
    json dati = json::parse(request.body());
    
    // Controlla se i dati forniti dall'utente sono presenti, non null e corretti
    if (!dati.contains("via") || dati["via"].is_null() || dati["via"].get<std::string>().empty()) 
        response.send(Pistache::Http::Code::Bad_Request, "Via not provided\n");
    
    if (!dati.contains("civico") || dati["civico"].is_null()) 
        response.send(Pistache::Http::Code::Bad_Request, "Civico not provided or not a number\n");
    
    if (!dati.contains("cap") || dati["cap"].is_null() || dati["cap"].get<std::string>().empty()) 
        response.send(Pistache::Http::Code::Bad_Request, "CAP not provided\n");
    
    if (!dati.contains("city") || dati["city"].is_null() || dati["city"].get<std::string>().empty()) 
        response.send(Pistache::Http::Code::Bad_Request, "City not provided\n");
    
    if (!dati.contains("stato") || dati["stato"].is_null() || dati["stato"].get<std::string>().empty()) 
        response.send(Pistache::Http::Code::Bad_Request, "State not provided\n");

    // Recupera i dati convertiti
    std::string via = dati["via"].get<std::string>();
    int civico = dati["civico"].get<int>();
    std::string CAP = dati["cap"].get<std::string>();
    std::string city = dati["city"].get<std::string>();
    std::string stato = dati["stato"].get<std::string>();

    // Verifica la lunghezza dei campi
    
    if (via.length() > 30)
    {
        response.send(Pistache::Http::Code::Bad_Request, "Via length is above 30 characters\n");
        return;
    }
    if (civico < 0)
    {
        response.send(Pistache::Http::Code::Bad_Request, "Civico must be a positive integer\n");
        return;
    }
    if (CAP.length() != 5 || !isNumber(CAP))
    {
        response.send(Pistache::Http::Code::Bad_Request, "CAP must be a string of 5 numbers\n");
        return;
    }
    if (city.length() > 30)
    {
        response.send(Pistache::Http::Code::Bad_Request, "City length is above 30 characters\n");
        return;
    }
    if (stato.length() > 50)
    {
        response.send(Pistache::Http::Code::Bad_Request, "State length is above 50 characters\n");
        return;
    }
    // Recupera l'ID del cliente basato sull'email
    int customerID = recuperaCustomerID(email);
    if (customerID <= 0) {
        response.send(Pistache::Http::Code::Internal_Server_Error, "Errore nel recupero dell'ID cliente\n");
        return;
    }
    logID = inserimentoOperazione(customerID, "Aggiunta di un indirizzo");
    if (logID == -1)
    {
        response.send(Pistache::Http::Code::Internal_Server_Error, "Errore nel sistema di monitoraggio\n");
        return;
    }

    // Chiama la funzione per creare il nuovo Fornitore
    bool esito = aggiungiindirizzo(customerID, via.c_str(), civico, CAP.c_str(), city.c_str(), stato.c_str());
    
    if (esito) {
        response.send(Pistache::Http::Code::Created, "Indirizzo Aggiunto\n");
        successoOperazione(logID);
    } else {
        response.send(Pistache::Http::Code::Internal_Server_Error, "Errore nell'aggiunta dell'Indirizzo\n");
        fallimentoOperazione(logID);
    }
}

//curl -X DELETE http://localhost:5001/abc@abc.it/carrello/1
void removeCarrello(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response) 
{
    int logID;
    char comando[100];
    // Recupera i parametri dalla richiesta
    std::string email = request.param(":email").as<std::string>();
    if (email.empty()) {
        response.send(Pistache::Http::Code::Bad_Request, "Email not provided\n");
        return;
    }
    int productID = request.param(":productID").as<int>();
    if (productID <= 0)
    {
        response.send(Pistache::Http::Code::Bad_Request, "ProductID not provided\n");
        return;
    }

    // Recupera l'ID del cliente basato sull'email
    int customerID = recuperaCustomerID(email);
    if (customerID <= 0) {
        response.send(Pistache::Http::Code::Internal_Server_Error, "Errore nel recupero dell'ID cliente\n");
        return;
    }
    sprintf(comando, "Rimozione del prodotto con ID = %d dal carrello", productID);
    logID = inserimentoOperazione(customerID, comando);
    if (logID == -1)
    {
        response.send(Pistache::Http::Code::Internal_Server_Error, "Errore nel sistema di monitoraggio\n");
        return;
    }

    bool esito = rimuoviCarrello(productID, customerID);
    if (esito) {
        response.send(Pistache::Http::Code::Ok, "Prodotto rimosso dal carrello con successo\n");
        successoOperazione(logID);
    } else {
        response.send(Pistache::Http::Code::Internal_Server_Error, "Errore durante la rimozione del prodotto dal carrello\n");
        fallimentoOperazione(logID);
    }
}

//curl -X DELETE http://localhost:5001/abc@abc.it/ordini/1
void annullaOrd(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response) 
{
    int logID;
    char comando[100];
    // Recupera i parametri dalla richiesta
    std::string email = request.param(":email").as<std::string>();
    if (email.empty()) {
        response.send(Pistache::Http::Code::Bad_Request, "Email not provided\n");
        return;
    }
    int ordineID = request.param(":ordineID").as<int>();
    if (ordineID <= 0)
    {
        response.send(Pistache::Http::Code::Bad_Request, "OrdineID not provided\n");
        return;
    }

    // Recupera l'ID del cliente basato sull'email
    int customerID = recuperaCustomerID(email);
    if (customerID <= 0) {
        response.send(Pistache::Http::Code::Internal_Server_Error, "Errore nel recupero dell'ID cliente\n");
        return;
    }
    sprintf(comando, "Annullamento dell'ordine con ID = %d", ordineID);
    logID = inserimentoOperazione(customerID, comando);
    if (logID == -1)
    {
        response.send(Pistache::Http::Code::Internal_Server_Error, "Errore nel sistema di monitoraggio\n");
        return;
    }

    bool esito = annullaOrdine(ordineID, customerID);
    if (esito) {
        response.send(Pistache::Http::Code::Ok, "Ordine annullato con successo\n");
        successoOperazione(logID);
    } else {
        response.send(Pistache::Http::Code::Internal_Server_Error, "Errore durante l'annullamento dell'ordine\n(Se l'ordine è già stato preso in carico, non è possibile annullarlo!)\n");
        fallimentoOperazione(logID);
    }
}

//curl -X PUT -H "Content-Type: application/json" -d '{"pagamento": "contante", "indirizzo": 1}' http://localhost:5001//abc@abc.it/ordini/
void ordina(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response) 
{
    int logID;
    // L'indirizzo proposto deve essere tra quelli presenti
    std::string OPZIONI[] = {"Virtuale", "contante", "carta prepagata", "carta di credito", "bancomat"};
    std::string email = request.param(":email").as<std::string>();
    json dati = json::parse(request.body());
    // Controlla se i dati forniti dall'utente sono presenti e corretti
    if (!dati.contains("pagamento") || dati["pagamento"].empty()) response.send(Pistache::Http::Code::Bad_Request, "Payment not provided\n");
    if (!dati.contains("indirizzo") || dati["indirizzo"].empty()) response.send(Pistache::Http::Code::Bad_Request, "Address not provided\n");
    
    std::string pagamento = dati["pagamento"];
    int indirizzo = dati["indirizzo"];
    if (!std::find(std::begin(OPZIONI), std::end(OPZIONI), pagamento)) response.send(Pistache::Http::Code::Bad_Request, "Payment is not in the system\n");
    if (indirizzo < 0) response.send(Pistache::Http::Code::Bad_Request, "Address must be a positive integer\n");

    // Recupera l'ID del cliente basato sull'email
    int customerID = recuperaCustomerID(email);
    if (customerID <= 0) 
    {
        response.send(Pistache::Http::Code::Internal_Server_Error, "Errore nel recupero dell'ID cliente");
        return;
    }
    logID = inserimentoOperazione(customerID, "Richiesta di un ordine");
    if (logID == -1)
    {
        response.send(Pistache::Http::Code::Internal_Server_Error, "Errore nel sistema di monitoraggio\n");
        return;
    }
    
    bool esito = effettuaOrdine(customerID, pagamento.c_str(), indirizzo);
    if (esito) {
        response.send(Pistache::Http::Code::Ok, "Ordine effettuato\n");
        successoOperazione(logID);
    } else {
        response.send(Pistache::Http::Code::Internal_Server_Error, "Errore durante l'ordine\n Verificare siano presenti prodotti nel carrello!\n");
        fallimentoOperazione(logID);
    }
}

//curl -X DELETE http://localhost:5001/abc@abc.it/indirizzi/1
void removeIndirizzo(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response) 
{
    int logID;
    char comando[100];
        // Recupera i parametri dalla richiesta
    std::string email = request.param(":email").as<std::string>();
    if (email.empty()) {
        response.send(Pistache::Http::Code::Bad_Request, "Email not provided\n");
        return;
    }
    int addressID = request.param(":addressID").as<int>();
    if (addressID <= 0)
    {
        response.send(Pistache::Http::Code::Bad_Request, "AddressID not provided\n");
        return;
    }
    int customerID = recuperaCustomerID(email);
    if (customerID <= 0) 
    {
        response.send(Pistache::Http::Code::Internal_Server_Error, "Errore nel recupero dell'ID cliente");
        return;
    }
    sprintf(comando, "Rimozione dell'indirizzo con ID = %d", addressID);
    logID = inserimentoOperazione(customerID, comando);
    if (logID == -1)
    {
        response.send(Pistache::Http::Code::Internal_Server_Error, "Errore nel sistema di monitoraggio\n");
        return;
    }

    // Elimina l'indirizzo con l'ID indicato (la rimozione da custaddr avviene in automatico tramite trigger del database)
    bool esito = rimuoviIndirizzo(addressID);
    if (esito) {
        response.send(Pistache::Http::Code::Ok, "Indirizzo rimosso con successo\n");
        successoOperazione(logID);
    } else {
        response.send(Pistache::Http::Code::Internal_Server_Error, "Errore nella rimozione dell'indirizzo\n");
        fallimentoOperazione(logID);
    }
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


int inserimentoOperazione(int customerID, const char* operazione)
{
    int logID;
    PGresult *res;
    char comando[1000];
    Con2DB db(HOSTNAME, DB_PORT, USERNAME_HANDLER, PASSWORD_HANDLER, LOG_DB_NAME); // Effettua la connessione al database dei log
    try
    {
        sprintf(comando, "INSERT INTO cliente(Cliente_Id, TipoUser, OperationType, Data_inizio) VALUES(%d, 'customer', '%s', NOW()) RETURNING Id", customerID, operazione);
        res = db.ExecSQLtuples(comando);
        logID = atoi(PQgetvalue(res, 0, PQfnumber(res, "Id")));
        PQclear(res);
        return logID;
    }
    catch(...)
    {
        std::cerr << "Errore nel database dei log\n";
        PQclear(res);
        return -1;
    }
}

bool successoOperazione(int logID)
{
    PGresult *res;
    char comando[1000];
    Con2DB db(HOSTNAME, DB_PORT, USERNAME_HANDLER, PASSWORD_HANDLER, LOG_DB_NAME); // Effettua la connessione al database dei log
    try
    {
        sprintf(comando, "UPDATE cliente SET Data_termine = NOW(), Esito = 'Successo' WHERE Id = %d", logID);
        res = db.ExecSQLcmd(comando);
        PQclear(res);
        return true;
    }
    catch(...)
    {
        std::cerr << "Errore nel database dei log\n";
        PQclear(res);
        return false;
    }

}

bool fallimentoOperazione(int logID)
{
    PGresult *res;
    char comando[1000];
    Con2DB db(HOSTNAME, DB_PORT, USERNAME_HANDLER, PASSWORD_HANDLER, LOG_DB_NAME); // Effettua la connessione al database dei log
    try
    {
        sprintf(comando, "UPDATE cliente SET Data_termine = NOW(), Esito = 'Fallito' WHERE Id = %d", logID);
        res = db.ExecSQLcmd(comando);
        PQclear(res);
        return true;
    }
    catch(...)
    {
        std::cerr << "Errore nel database dei log\n";
        PQclear(res);
        return false;
    }

}