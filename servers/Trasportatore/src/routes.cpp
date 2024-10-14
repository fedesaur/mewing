#include "routes.h"
using json = nlohmann::json; // Abbreviazione per il json
using namespace std::chrono_literals; // Namespace per utilizzare il timer
using Clock = std::chrono::system_clock;

void defineRoutes(Pistache::Rest::Router& router) 
{
    // Registrazione delle rotte con funzioni globali
    Pistache::Rest::Routes::Get(router, "/autentica/:piva", Pistache::Rest::Routes::bind(&autenticaTrasportatore));
    Pistache::Rest::Routes::Get(router, "/:piva/ricerca/", Pistache::Rest::Routes::bind(&getOrdini));
    Pistache::Rest::Routes::Get(router, "/:piva/corrieri/", Pistache::Rest::Routes::bind(&getCorrieri));
    Pistache::Rest::Routes::Get(router, "/:piva/correnti/", Pistache::Rest::Routes::bind(&getCorrenti));
    Pistache::Rest::Routes::Get(router, "/:piva/registrati/", Pistache::Rest::Routes::bind(&getRegistrati));
    Pistache::Rest::Routes::Put(router, "/:piva/corrieri/", Pistache::Rest::Routes::bind(&putCorriere));
    Pistache::Rest::Routes::Put(router, "/:piva/ordini/", Pistache::Rest::Routes::bind(&accettaOrdine));
    Pistache::Rest::Routes::Put(router, "/autentica/", Pistache::Rest::Routes::bind(&creaTrasportatore));
    Pistache::Rest::Routes::Post(router, "/:piva/ordini/:orderID", Pistache::Rest::Routes::bind(&consegna));
    Pistache::Rest::Routes::Delete(router, "/:piva/corrieri/:courierID", Pistache::Rest::Routes::bind(&deleteCorriere));
}

//curl -X GET http://localhost:5003/autentica/32132132132
void autenticaTrasportatore(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response)
{
    auto start = std::chrono::high_resolution_clock::now(); // Memorizza il tempo d'inizio dell'operazione
    // Recupera l'email dal percorso
    std::string IVA = request.param(":piva").as<std::string>();

    if (IVA.empty() || IVA.length() != 11) {
        response.send(Pistache::Http::Code::Bad_Request, "IVA not provided");
        return;
    }
    // Ora chiama la funzione autentica
    int ID = autentica(IVA.c_str());
    auto finish = std::chrono::high_resolution_clock::now(); // Memorizza il tempo di fine dell'operazione
    double elapsed = std::chrono::duration_cast<std::chrono::duration<double>> (finish-start).count();
    std::cout << elapsed << std::endl;
    if (elapsed > TEMPO_LIMITE) // Se il tempo dell'operazione è superiore al tempo limite, viene ritornato un timeout
    {
        response.send(Pistache::Http::Code::Internal_Server_Error, "La richiesta ha necessitato troppo tempo\n");
        return;
    }
    if (ID > 0) {
        response.send(Pistache::Http::Code::Ok, "Trasporter authenticated\n");
    } else if (ID == 0) {
        response.send(Pistache::Http::Code::Not_Found, "User isn't in the system. You have to create it first\n");
    } else {
        response.send(Pistache::Http::Code::Unauthorized, "Authentication failed\n");
    }
}

//curl -X PUT -H "Content-Type: application/json" -d '{"nome": "AmaZZon", "piva": "12345678912", "via" : "Via del Denaro", "civico": 7, "cap" : "77777", "city" : "Napoli", "stato" : "Montecarlo"}' http://localhost:5003/autentica/
void creaTrasportatore(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response)
{
    auto start = std::chrono::high_resolution_clock::now(); // Memorizza il tempo d'inizio dell'operazione
    json dati = json::parse(request.body());
    
    // Controlla se i dati forniti dall'utente sono presenti, non null e corretti
    if (!dati.contains("piva") || dati["piva"].empty())
    {
        response.send(Pistache::Http::Code::Bad_Request, "IVA not provided\n");
        return;
    } 
    if (!dati.contains("nome") || dati["nome"].empty())
    {
        response.send(Pistache::Http::Code::Bad_Request, "Name not provided\n");
        return;
    }
    if (!dati.contains("via") || dati["via"].empty())
    {
        response.send(Pistache::Http::Code::Bad_Request, "Via not provided\n");
        return;
    } 
    if (!dati.contains("civico") || dati["civico"].empty())
    {
        response.send(Pistache::Http::Code::Bad_Request, "Civico not provided or not a number\n");
        return;
    }
    if (!dati.contains("cap") || dati["cap"].empty()) 
    {
        response.send(Pistache::Http::Code::Bad_Request, "CAP not provided\n");
        return;
    }
    if (!dati.contains("city") || dati["city"].empty())
    {
        response.send(Pistache::Http::Code::Bad_Request, "City not provided\n");
        return;
    }
    if (!dati.contains("stato") || dati["stato"].empty())
    {
        response.send(Pistache::Http::Code::Bad_Request, "State not provided\n");
        return;
    }
    std::string nome = dati["nome"];
    std::string piva = dati["piva"];
    std::string via = dati["via"];
    int civico = dati["civico"];
    std::string CAP = dati["cap"];
    std::string city = dati["city"];
    std::string stato = dati["stato"];
    // Verifica la lunghezza dei campi
    if (piva.length() != 11 || !isNumber(piva))
    {
        response.send(Pistache::Http::Code::Bad_Request, "IVA must be a sequence of 11 numbers\n");
        return;
    } 
    if (nome.length() > 100)
    {
        response.send(Pistache::Http::Code::Bad_Request, "Name length is above 100 characters\n");
        return;
    }
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

    // Chiama la funzione per creare il nuovo Trasportatore
    bool esito = crea(piva.c_str(), nome.c_str(), via.c_str(), civico, CAP.c_str(), city.c_str(), stato.c_str());
    auto finish = std::chrono::high_resolution_clock::now(); // Memorizza il tempo di fine dell'operazione
    double elapsed = std::chrono::duration_cast<std::chrono::duration<double>> (finish-start).count();
    std::cout << elapsed << std::endl;
    if (elapsed > TEMPO_LIMITE) // Se il tempo dell'operazione è superiore al tempo limite, viene ritornato un timeout
    {
        response.send(Pistache::Http::Code::Internal_Server_Error, "La richiesta ha necessitato troppo tempo\n");
        return;
    }
    if (esito) {
        response.send(Pistache::Http::Code::Created, "Trasporter created\n");
    } else {
        response.send(Pistache::Http::Code::Unauthorized, "Failed to create the Trasporter\n");
    }
}


//curl -X GET http://localhost:5003/32132132132/ricerca/
void getOrdini(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response)
{
    auto start = std::chrono::high_resolution_clock::now(); // Memorizza il tempo d'inizio dell'operazione
    int logID;
    std::stringstream ss;
    redisContext *c2r; // c2r contiene le info sul contesto
    redisReply *reply; // reply contiene le risposte da Redis
    redisReply *orderReply;
    Ordine* ORDINI;
    Indirizzo* INDIRIZZI;
    int RIGHE;

    std::string IVA = request.param(":piva").as<std::string>();
    
    // Controlla che i parametri richiesti siano stati forniti
    if (IVA.empty() || IVA.length() != 11) {
        response.send(Pistache::Http::Code::Bad_Request, "IVA not provided\n");
        return;
    }
    int trasporterID = recuperaCourierID(IVA); //Controlla che l'utente è nel sistema
    if (trasporterID <= 0)
    {
        response.send(Pistache::Http::Code::Unauthorized, "Il Trasportatore non è nel sistema!\n");
        return;
    }
    logID = inserimentoOperazione(trasporterID , "Ricerca degli ordini disponibili");
    if (logID == -1)
    {
        response.send(Pistache::Http::Code::Internal_Server_Error, "Errore nel sistema di monitoraggio\n");
        return;
    }
    bool pubblicati = ricercaOrdini(IVA.c_str()); // Immette i prodotti presenti nel carrello nello stream
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

    // Recupera la lista degli ordini da Redis
    reply = RedisCommand(c2r, "LRANGE ordini:%s 0 -1", IVA.c_str());
    if (reply->type == REDIS_REPLY_ARRAY && reply->elements > 0) // Recupera gli indirizzi da Redis
    {
        RIGHE = reply->elements;
        ORDINI = new Ordine[RIGHE];  // Array dinamico di prodotti
        INDIRIZZI = new Indirizzo[RIGHE];
        
        for (int i = 0; i < RIGHE; i++) 
        {
            std::string orderID = reply->element[i]->str;

            // Recupera il prodotto come hash da Redis
            orderReply = RedisCommand(c2r, "HGETALL ordine:%s", orderID.c_str());
    
            // Verifica il risultato del recupero...
            if (orderReply->type == REDIS_REPLY_ARRAY) // 7 dati Richiesti: ID Ordine, Stato, Data Richiesta, Data Consegna, Totale, Tipo di Pagamento, Indirizzo
            { //... e asssocia i valori dell'indirizzo recuperato dallo stream Redis ad un oggetto Indirizzo 
            
                ORDINI[i].ID = std::atoi(orderReply->element[1]->str);
                ORDINI[i].MailCustomer = (orderReply->element[3]->str);
                ORDINI[i].DataRichiesta = (orderReply->element[5]->str);
                ORDINI[i].Totale = std::atof(orderReply->element[7]->str);
                ORDINI[i].Pagamento = (orderReply->element[9]->str);
                
                INDIRIZZI[i].via = (orderReply->element[11]->str);
                INDIRIZZI[i].civico = std::atoi(orderReply->element[13]->str);
                INDIRIZZI[i].CAP = (orderReply->element[15]->str);
                INDIRIZZI[i].citta = (orderReply->element[17]->str);
                INDIRIZZI[i].stato = (orderReply->element[19]->str);
            } else {
                delete[] ORDINI; // Libera la memoria allocata dinamicamente
                delete[] INDIRIZZI; // Libera la memoria allocata dinamicamente
                std::cerr << "Errore nel recupero di un ordine da Redis" << std::endl;
                fallimentoOperazione(logID);
                freeReplyObject(orderReply);
                redisFree(c2r);
                return;
            }
        }
        // Stampa gli ordini disponibili
        ss << "\nORDINI DISPONIBILI:\n";
        for (int i = 0; i < RIGHE; i++) 
        {
            ss << i+1 << ") ID Ordine: " << ORDINI[i].ID
                << " Mail Customer: " << ORDINI[i].MailCustomer
                << " Data Richiesta: " << ORDINI[i].DataRichiesta
                << " Metodo Pagamento: " << ORDINI[i].Pagamento
                << " Totale Ordine: " << ORDINI[i].Totale
                << " Via della Consegna: " << INDIRIZZI[i].via
                << " Civico: " << INDIRIZZI[i].civico
                << " CAP: " << INDIRIZZI[i].CAP
                << " Città della Consegna: " << INDIRIZZI[i].citta 
                << " Stato della Città: " << INDIRIZZI[i].stato << "\n";
        }
        // Invia la risposta con i prodotti
        response.send(Pistache::Http::Code::Ok, ss.str());
        delete[] ORDINI; // Libera la memoria allocata dinamicamente
        delete[] INDIRIZZI; // Libera la memoria allocata dinamicamente
    } else {
        // Nessun prodotto trovato in Redis
        response.send(Pistache::Http::Code::Ok, "Nessun ordine disponibile!\n");
    }
    auto finish = std::chrono::high_resolution_clock::now(); // Memorizza il tempo di fine dell'operazione
    double elapsed = std::chrono::duration_cast<std::chrono::duration<double>> (finish-start).count();
    std::cout << elapsed << std::endl;
    if (elapsed > TEMPO_LIMITE) // Se il tempo dell'operazione è superiore al tempo limite, viene ritornato un timeout
    {
        response.send(Pistache::Http::Code::Internal_Server_Error, "La richiesta ha necessitato troppo tempo\n");
        fallimentoOperazione(logID);
        return;
    }
    successoOperazione(logID);
    freeReplyObject(reply);
    redisFree(c2r);  // Chiudi la connessione a Redis
    return;
}

//curl -X GET http://localhost:5003/32132132132/correnti/
void getCorrenti(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response)
{
    auto start = std::chrono::high_resolution_clock::now(); // Memorizza il tempo d'inizio dell'operazione
    int logID;
    std::stringstream ss;
    redisContext *c2r; // c2r contiene le info sul contesto
    redisReply *reply; // reply contiene le risposte da Redis
    redisReply *orderReply;
    Ordine* ORDINI;
    Corriere* CORRIERI;
    int RIGHE;

    std::string IVA = request.param(":piva").as<std::string>();
    
    // Controlla che i parametri richiesti siano stati forniti
    if (IVA.empty() || IVA.length() != 11) {
        response.send(Pistache::Http::Code::Bad_Request, "IVA not provided\n");
        return;
    }
    int trasporterID = recuperaCourierID(IVA);
    if (trasporterID <= 0) {
        response.send(Pistache::Http::Code::Internal_Server_Error, "Errore nel recupero dell'ID del Trasportatore\n");
        return;
    }
    logID = inserimentoOperazione(trasporterID, "Recupero degli ordini in corso");
    if (logID == -1)
    {
        response.send(Pistache::Http::Code::Internal_Server_Error, "Errore nel sistema di monitoraggio\n");
        return;
    }
    bool pubblicati = ordiniCorrenti(IVA.c_str(), trasporterID); // Immette i prodotti presenti nel carrello nello stream
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

    // Recupera la lista degli ordini da Redis
    reply = RedisCommand(c2r, "LRANGE correnti:%s 0 -1", IVA.c_str());
    if (reply->type == REDIS_REPLY_ARRAY && reply->elements > 0) // Recupera gli indirizzi da Redis
    {
        RIGHE = reply->elements;
        ORDINI = new Ordine[RIGHE];  // Array dinamico di prodotti
        CORRIERI = new Corriere[RIGHE];
        
        for (int i = 0; i < RIGHE; i++) 
        {
            std::string orderID = reply->element[i]->str;

            // Recupera il prodotto come hash da Redis
            orderReply = RedisCommand(c2r, "HGETALL corrente:%s", orderID.c_str());
    
            // Verifica il risultato del recupero...
            if (orderReply->type == REDIS_REPLY_ARRAY) // 7 dati Richiesti: ID Ordine, Stato, Data Richiesta, Data Consegna, Totale, Tipo di Pagamento, Indirizzo
            { //... e asssocia i valori dell'indirizzo recuperato dallo stream Redis ad un oggetto Indirizzo 
            
                ORDINI[i].ID = std::atoi(orderReply->element[1]->str);
                ORDINI[i].MailCustomer = (orderReply->element[3]->str);
                ORDINI[i].DataRichiesta = (orderReply->element[5]->str);
                ORDINI[i].Totale = std::atof(orderReply->element[7]->str);
                ORDINI[i].Pagamento = (orderReply->element[9]->str);
                ORDINI[i].Stato = (orderReply->element[11]->str);
                
                CORRIERI[i].ID = std::atoi(orderReply->element[13]->str);
                CORRIERI[i].nome = (orderReply->element[15]->str);
                CORRIERI[i].cognome = (orderReply->element[17]->str);
            } else {
                std::cerr << "Errore nel recupero di un ordine da Redis" << std::endl;
                delete[] ORDINI; // Libera la memoria allocata dinamicamente
                delete[] CORRIERI; // Libera la memoria allocata dinamicamente
                response.send(Pistache::Http::Code::Internal_Server_Error, "Errore nel recupero degli ordini\n");
                fallimentoOperazione(logID);
                freeReplyObject(orderReply);
                redisFree(c2r);
                return;
            }
        }
        // Stampa gli ordini disponibili
        ss << "\nORDINI IN CORSO:\n";
        for (int i = 0; i < RIGHE; i++) 
        {
            ss << i+1 << ") ID Ordine: " << ORDINI[i].ID
                << " Mail Customer: " << ORDINI[i].MailCustomer
                << " Data Richiesta: " << ORDINI[i].DataRichiesta
                << " Metodo Pagamento: " << ORDINI[i].Pagamento
                << " Totale Ordine: " << ORDINI[i].Totale
                << " Stato dell'Ordine: " << ORDINI[i].Stato
                << "\n Preso in carico da : " << CORRIERI[i].nome << " " 
                << CORRIERI[i].cognome << " (ID : " << CORRIERI[i].ID << ")\n";
        }
        // Invia la risposta con gli ordini
        delete[] ORDINI; // Libera la memoria allocata dinamicamente
        delete[] CORRIERI; // Libera la memoria allocata dinamicamente
        auto finish = std::chrono::high_resolution_clock::now(); // Memorizza il tempo di fine dell'operazione
        double elapsed = std::chrono::duration_cast<std::chrono::duration<double>> (finish-start).count();
        std::cout << elapsed << std::endl;
        if (elapsed > TEMPO_LIMITE) // Se il tempo dell'operazione è superiore al tempo limite, viene ritornato un timeout
        {
            response.send(Pistache::Http::Code::Internal_Server_Error, "La richiesta ha necessitato troppo tempo\n");
            fallimentoOperazione(logID);
            return;
        }
        response.send(Pistache::Http::Code::Ok, ss.str());
    } else {
        response.send(Pistache::Http::Code::Ok, "Nessun ordine in corso!\n"); // Nessun prodotto trovato in Redis
    }
    auto finish = std::chrono::high_resolution_clock::now(); // Memorizza il tempo di fine dell'operazione
    double elapsed = std::chrono::duration_cast<std::chrono::duration<double>> (finish-start).count();
    std::cout << elapsed << std::endl;
    if (elapsed > TEMPO_LIMITE) // Se il tempo dell'operazione è superiore al tempo limite, viene ritornato un timeout
    {
        response.send(Pistache::Http::Code::Internal_Server_Error, "La richiesta ha necessitato troppo tempo\n");
        fallimentoOperazione(logID);
        return;
    }
    successoOperazione(logID);
    freeReplyObject(reply);
    redisFree(c2r);  // Chiudi la connessione a Redis
    return;
}

//curl -X GET http://localhost:5003/32132132132/registrati/
void getRegistrati(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response)
{
    auto start = std::chrono::high_resolution_clock::now(); // Memorizza il tempo d'inizio dell'operazione
    int logID;
    std::stringstream ss;
    redisContext *c2r; // c2r contiene le info sul contesto
    redisReply *reply; // reply contiene le risposte da Redis
    redisReply *orderReply;
    Ordine* ORDINI;
    Corriere* CORRIERI;
    int RIGHE;

    std::string IVA = request.param(":piva").as<std::string>();
    
    // Controlla che i parametri richiesti siano stati forniti
    if (IVA.empty() || IVA.length() != 11) {
        response.send(Pistache::Http::Code::Bad_Request, "IVA not provided\n");
        return;
    }
    int trasporterID = recuperaCourierID(IVA);
    if (trasporterID <= 0) {
        response.send(Pistache::Http::Code::Internal_Server_Error, "Errore nel recupero dell'ID del Trasportatore\n");
        return;
    }
    logID = inserimentoOperazione(trasporterID, "Recupero degli ordini registrati");
    if (logID == -1)
    {
        response.send(Pistache::Http::Code::Internal_Server_Error, "Errore nel sistema di monitoraggio\n");
        return;
    }

    bool pubblicati = registroOrdini(IVA.c_str(), trasporterID); // Immette i prodotti presenti nel carrello nello stream
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

    // Recupera la lista degli ordini da Redis
    reply = RedisCommand(c2r, "LRANGE registroOrdini:%s 0 -1", IVA.c_str());
    if (reply->type == REDIS_REPLY_ARRAY && reply->elements > 0) // Recupera gli indirizzi da Redis
    {
        RIGHE = reply->elements;
        ORDINI = new Ordine[RIGHE];  // Array dinamico di ordini
        CORRIERI = new Corriere[RIGHE];
        
        for (int i = 0; i < RIGHE; i++) 
        {
            std::string orderID = reply->element[i]->str;

            // Recupera il prodotto come hash da Redis
            orderReply = RedisCommand(c2r, "HGETALL ordineRegistrato:%s", orderID.c_str());
    
            // Verifica il risultato del recupero...
            if (orderReply->type == REDIS_REPLY_ARRAY && orderReply->elements == 20)
            { //... e asssocia i valori dell'indirizzo recuperato dallo stream Redis ad un oggetto Indirizzo 
            
                ORDINI[i].ID = std::atoi(orderReply->element[1]->str);
                ORDINI[i].MailCustomer = (orderReply->element[3]->str);
                ORDINI[i].Stato = (orderReply->element[5]->str);
                ORDINI[i].DataRichiesta = (orderReply->element[7]->str);
                ORDINI[i].DataConsegna = (orderReply->element[9]->str);
                ORDINI[i].Totale = std::atof(orderReply->element[11]->str);
                ORDINI[i].Pagamento = (orderReply->element[13]->str);

                CORRIERI[i].ID = std::atoi(orderReply->element[15]->str);
                CORRIERI[i].nome = (orderReply->element[17]->str);
                CORRIERI[i].cognome = (orderReply->element[19]->str);
            } else {
                delete[] ORDINI; // Libera la memoria allocata dinamicamente
                delete[] CORRIERI; // Libera la memoria allocata dinamicamente
                std::cerr << "Errore nel recupero di un ordine da Redis" << std::endl;
                response.send(Pistache::Http::Code::Internal_Server_Error, "Errore nel recupero degli ordini\n");
                fallimentoOperazione(logID);
                freeReplyObject(orderReply);
                redisFree(c2r);
                return;
            }
        }
        // Stampa gli ordini disponibili
        ss << "\nORDINI REGISTRATI:\n";
        for (int i = 0; i < RIGHE; i++) 
        {
            ss << i+1 << ") ID Ordine: " << ORDINI[i].ID
                << " Mail Customer: " << ORDINI[i].MailCustomer
                << " Data Richiesta: " << ORDINI[i].DataRichiesta
                << " Data Consegna: (0 se non consegnato) " << ORDINI[i].DataConsegna
                << " Metodo Pagamento: " << ORDINI[i].Pagamento
                << " Totale Ordine: " << ORDINI[i].Totale
                << " Stato dell'Ordine: " << ORDINI[i].Stato
                << "\n Preso in carico da : " << CORRIERI[i].nome << " " 
                << CORRIERI[i].cognome << " (ID : " << CORRIERI[i].ID << ")\n";
        }
        // Invia la risposta con gli ordini
        response.send(Pistache::Http::Code::Ok, ss.str());
        delete[] ORDINI; // Libera la memoria allocata dinamicamente
        delete[] CORRIERI; // Libera la memoria allocata dinamicamente
    } else {
        response.send(Pistache::Http::Code::Ok, "Nessun ordine in archivio!\n"); // Nessun prodotto trovato in Redis
    }
    auto finish = std::chrono::high_resolution_clock::now(); // Memorizza il tempo di fine dell'operazione
    double elapsed = std::chrono::duration_cast<std::chrono::duration<double>> (finish-start).count();
    std::cout << elapsed << std::endl;
    if (elapsed > TEMPO_LIMITE) // Se il tempo dell'operazione è superiore al tempo limite, viene ritornato un timeout
    {
        response.send(Pistache::Http::Code::Internal_Server_Error, "La richiesta ha necessitato troppo tempo\n");
        fallimentoOperazione(logID);
        return;
    }
    successoOperazione(logID);
    freeReplyObject(reply);
    redisFree(c2r);  // Chiudi la connessione a Redis
    return;
}

//curl -X GET http://localhost:5003/32132132132/corrieri/
void getCorrieri(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response)
{
    auto start = std::chrono::high_resolution_clock::now(); // Memorizza il tempo d'inizio dell'operazione
    int logID;
    std::stringstream ss;
    redisContext *c2r; // c2r contiene le info sul contesto
    redisReply *reply; // reply contiene le risposte da Redis
    redisReply *courierReply;
    Corriere* CORRIERI;
    int RIGHE;

    std::string IVA = request.param(":piva").as<std::string>();
    
    // Controlla che i parametri richiesti siano stati forniti
    if (IVA.empty() || IVA.length() != 11) {
        response.send(Pistache::Http::Code::Bad_Request, "IVA not provided\n");
        return;
    }

    int trasporterID = recuperaCourierID(IVA);
    if (trasporterID <= 0) {
        response.send(Pistache::Http::Code::Internal_Server_Error, "Errore nel recupero dell'ID del Trasportatore\n");
        return;
    }
    logID = inserimentoOperazione(trasporterID, "Recupero dei corrieri registrati");
    if (logID == -1)
    {
        response.send(Pistache::Http::Code::Internal_Server_Error, "Errore nel sistema di monitoraggio\n");
        return;
    }
    bool pubblicati = recuperaCorrieri(IVA.c_str(), trasporterID); // Immette i prodotti presenti nel carrello nello stream
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

    // Recupera la lista degli ordini da Redis
    reply = RedisCommand(c2r, "LRANGE corrieri:%s 0 -1", IVA.c_str());
    if (reply->type == REDIS_REPLY_ARRAY && reply->elements > 0) // Recupera gli indirizzi da Redis
    {
        RIGHE = reply->elements;
        CORRIERI = new Corriere[RIGHE];  // Array dinamico di corrieri
        
        for (int i = 0; i < RIGHE; i++) 
        {
            std::string courierID = reply->element[i]->str;

            // Recupera il prodotto come hash da Redis
            courierReply = RedisCommand(c2r, "HGETALL corriere:%s", courierID.c_str());
    
            // Verifica il risultato del recupero...
            if (courierReply->type == REDIS_REPLY_ARRAY && courierReply->elements == 6) // 7 dati Richiesti: ID Ordine, Stato, Data Richiesta, Data Consegna, Totale, Tipo di Pagamento, Indirizzo
            { //... e asssocia i valori dell'indirizzo recuperato dallo stream Redis ad un oggetto Indirizzo 
            
                CORRIERI[i].ID = std::atoi(courierReply->element[1]->str);
                CORRIERI[i].nome = (courierReply->element[3]->str);
                CORRIERI[i].cognome = (courierReply->element[5]->str);
            } else {
                delete[] CORRIERI; // Libera la memoria allocata dinamicamente
                std::cerr << "Errore nel recupero di un ordine da Redis" << std::endl;
                fallimentoOperazione(logID);
                freeReplyObject(courierReply);
                redisFree(c2r);
                return;
            }
        }
        // Stampa gli ordini disponibili
        ss << "\nCORRIERI REGISTRATI:\n";
        for (int i = 0; i < RIGHE; i++) 
        {
            ss << i+1 << ") ID Corriere: " << CORRIERI[i].ID
                << " Nome Corriere: " << CORRIERI[i].nome
                << " Cognome Corriere: " << CORRIERI[i].cognome  << "\n";
        }
        // Invia la risposta con i prodotti
        response.send(Pistache::Http::Code::Ok, ss.str());
        delete[] CORRIERI; // Libera la memoria allocata dinamicamente
    } else {
        // Nessun prodotto trovato in Redis
        response.send(Pistache::Http::Code::Ok, "Nessun corriere registrato!\n");
    }
    auto finish = std::chrono::high_resolution_clock::now(); // Memorizza il tempo di fine dell'operazione
    double elapsed = std::chrono::duration_cast<std::chrono::duration<double>> (finish-start).count();
    std::cout << elapsed << std::endl;
    if (elapsed > TEMPO_LIMITE) // Se il tempo dell'operazione è superiore al tempo limite, viene ritornato un timeout
    {
        response.send(Pistache::Http::Code::Internal_Server_Error, "La richiesta ha necessitato troppo tempo\n");
        fallimentoOperazione(logID);
        return;
    }
    successoOperazione(logID);
    freeReplyObject(reply);
    redisFree(c2r);  // Chiudi la connessione a Redis
    return;
}

//curl -X POST http://localhost:5003/32132132132/ordini/1
void consegna(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response)
{
    auto start = std::chrono::high_resolution_clock::now(); // Memorizza il tempo d'inizio dell'operazione
    int logID;
    char comando[100];
    // Recupera l'email dal percorso
    std::string IVA = request.param(":piva").as<std::string>();
    std::string orderID = request.param(":orderID").as<std::string>();

    // Controlla che i parametri richiesti siano stati forniti
    if (IVA.empty() || IVA.length() != 11) {
        response.send(Pistache::Http::Code::Bad_Request, "IVA not provided\n");
        return;
    } else if (orderID.empty() ){
        response.send(Pistache::Http::Code::Bad_Request, "OrderID not provided\n");
        return;
    }
    int ID = stoi(orderID);
    int trasporterID = recuperaCourierID(IVA);
    if (trasporterID <= 0) {
        response.send(Pistache::Http::Code::Internal_Server_Error, "Errore nel recupero dell'ID del Trasportatore\n");
        return;
    }
    sprintf(comando, "Consegna ordine con ID = %d", ID);
    logID = inserimentoOperazione(trasporterID, comando);
    if (logID == -1)
    {
        response.send(Pistache::Http::Code::Internal_Server_Error, "Errore nel sistema di monitoraggio\n");
        return;
    }
    bool esito = consegnaOrdine(ID);
    auto finish = std::chrono::high_resolution_clock::now(); // Memorizza il tempo di fine dell'operazione
    double elapsed = std::chrono::duration_cast<std::chrono::duration<double>> (finish-start).count();
    std::cout << elapsed << std::endl;
    if (elapsed > TEMPO_LIMITE) // Se il tempo dell'operazione è superiore al tempo limite, viene ritornato un timeout
    {
        response.send(Pistache::Http::Code::Internal_Server_Error, "La richiesta ha necessitato troppo tempo\n");
        fallimentoOperazione(logID);
        return;
    }
    if (esito) {
        response.send(Pistache::Http::Code::Ok, "Ordine consegnato\n");
        successoOperazione(logID);
    } else {
        response.send(Pistache::Http::Code::Unauthorized, "C'è stato un errore nella consegna\n (Controllare che l'ordine non sia già stato consegnato!)\n");
        fallimentoOperazione(logID);
    }
}

//curl -X PUT -H "Content-Type: application/json" -d '{"nome": "Eugenio", "cognome": "Montale"}' http://localhost:5003/32132132132/corrieri/
void putCorriere(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response)
{
    auto start = std::chrono::high_resolution_clock::now(); // Memorizza il tempo d'inizio dell'operazione
    int logID;
    // Recupera l'email del trasportatore dai parametri
    std::string IVA = request.param(":piva").as<std::string>();
    if (IVA.empty() || IVA.length() != 11) {
        response.send(Pistache::Http::Code::Bad_Request, "IVA not provided");
        return;
    }

    json dati = json::parse(request.body());
    // Controlla se i dati forniti dall'utente sono presenti e corretti
    if (!dati.contains("nome") || dati["nome"].empty())
    {
        response.send(Pistache::Http::Code::Bad_Request, "Courier's name not provided\n");
        return;
    } 
    if (!dati.contains("cognome") || dati["cognome"].empty())
    {
        response.send(Pistache::Http::Code::Bad_Request, "Courier's surname not provided\n");
        return;
    }
    std::string nome = dati["nome"];
    std::string cognome = dati["cognome"];
    if (nome.length() > 20)
    {
        response.send(Pistache::Http::Code::Bad_Request, "Name length is above 20 characters\n");
        return;
    }
    if (cognome.length() > 20)
    {
        response.send(Pistache::Http::Code::Bad_Request, "Surname length is above 20 characters\n");
        return;
    }

    int trasporterID = recuperaCourierID(IVA);
    if (trasporterID <= 0) {
        response.send(Pistache::Http::Code::Internal_Server_Error, "Errore nel recupero dell'ID del Trasportatore\n");
        return;
    }
    logID = inserimentoOperazione(trasporterID, "Inserimento di un nuovo corriere");
    if (logID == -1)
    {
        response.send(Pistache::Http::Code::Internal_Server_Error, "Errore nel sistema di monitoraggio\n");
        return;
    }
    bool esito = registraCorriere(trasporterID, nome.c_str(), cognome.c_str());
    auto finish = std::chrono::high_resolution_clock::now(); // Memorizza il tempo di fine dell'operazione
    double elapsed = std::chrono::duration_cast<std::chrono::duration<double>> (finish-start).count();
    std::cout << elapsed << std::endl;
    if (elapsed > TEMPO_LIMITE) // Se il tempo dell'operazione è superiore al tempo limite, viene ritornato un timeout
    {
        response.send(Pistache::Http::Code::Internal_Server_Error, "La richiesta ha necessitato troppo tempo\n");
        fallimentoOperazione(logID);
        return;
    }
    if (esito) {
        response.send(Pistache::Http::Code::Created, "Courier added to system\n");
        successoOperazione(logID);
    } else {
        response.send(Pistache::Http::Code::Unauthorized, "Failed to add the courier to system\n");
        fallimentoOperazione(logID);
    }
}

//curl -X PUT -H "Content-Type: application/json" -d '{"ordine": 1, "corriere": 1}' http://localhost:5003/32132132132/ordini/
void accettaOrdine(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response)
{
    auto start = std::chrono::high_resolution_clock::now(); // Memorizza il tempo d'inizio dell'operazione
    int logID;
    char comando[100];
    std::string IVA = request.param(":piva").as<std::string>();

    if (IVA.empty() || IVA.length() != 11) {
        response.send(Pistache::Http::Code::Bad_Request, "IVA not provided");
        return;
    }

    json dati = json::parse(request.body());

    if (!dati.contains("ordine") || dati["ordine"].empty())
    {
        response.send(Pistache::Http::Code::Bad_Request, "OrderID not provided\n");
        return;
    }
    if (!dati.contains("corriere") || dati["corriere"].empty())
    {
        response.send(Pistache::Http::Code::Bad_Request, "CourierID not provided\n");
        return;
    }

    int orderID = dati["ordine"];
    int courierID = dati["corriere"];
    if (orderID <= 0)
    {
        response.send(Pistache::Http::Code::Bad_Request, "OrderID must be a positive integer\n");
        return;
    }
    if (courierID <= 0)
    {
        response.send(Pistache::Http::Code::Bad_Request, "CourierID must be a positive integer\n");
        return;
    }

    // Recupera l'ID del cliente basato sull'email
    int trasporterID = recuperaCourierID(IVA);
    if (trasporterID <= 0) {
        response.send(Pistache::Http::Code::Internal_Server_Error, "Errore nel recupero dell'ID del Trasportatore\n");
        return;
    }
    sprintf(comando, "Presa in carico ordine con ID = %d", orderID);
    logID = inserimentoOperazione(trasporterID, comando);
    if (logID == -1)
    {
        response.send(Pistache::Http::Code::Internal_Server_Error, "Errore nel sistema di monitoraggio\n");
        return;
    }
    
    bool esito = prendiOrdine(trasporterID, courierID, orderID);
    auto finish = std::chrono::high_resolution_clock::now(); // Memorizza il tempo di fine dell'operazione
    double elapsed = std::chrono::duration_cast<std::chrono::duration<double>> (finish-start).count();
    std::cout << elapsed << std::endl;
    if (elapsed > TEMPO_LIMITE) // Se il tempo dell'operazione è superiore al tempo limite, viene ritornato un timeout
    {
        response.send(Pistache::Http::Code::Internal_Server_Error, "La richiesta ha necessitato troppo tempo\n");
        fallimentoOperazione(logID);
        return;
    }
    if (esito) {
        response.send(Pistache::Http::Code::Ok, "Ordine preso in carico\n");
        successoOperazione(logID);
    } else {
        response.send(Pistache::Http::Code::Internal_Server_Error, "Errore nel prendere in carico l'ordine\n (Un altro trasportatore potrebbe averlo già preso in carico)\n");
        fallimentoOperazione(logID);
    }
}

//curl -X DELETE http://localhost:5003/32132132132/corrieri/1
void deleteCorriere(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response)
{
    auto start = std::chrono::high_resolution_clock::now(); // Memorizza il tempo d'inizio dell'operazione
    int logID;
    char comando[100];
    // Recupera l'email dal percorso
    std::string IVA = request.param(":piva").as<std::string>();
    std::string courierID = request.param(":courierID").as<std::string>();

    // Controlla che i parametri richiesti siano stati forniti
    if (IVA.empty() || IVA.length() != 11) {
        response.send(Pistache::Http::Code::Bad_Request, "IVA not provided\n");
        return;
    } 
    if (courierID.empty() ){
        response.send(Pistache::Http::Code::Bad_Request, "CourierID not provided\n");
        return;
    }
    int ID = stoi(courierID);
    int trasporterID = recuperaCourierID(IVA);
    if (trasporterID <= 0) {
        response.send(Pistache::Http::Code::Internal_Server_Error, "Errore nel recupero dell'ID del Trasportatore\n");
        return;
    }
    sprintf(comando, "Rimozione del corriere con ID = %d", ID);
    logID = inserimentoOperazione(trasporterID, comando);
    if (logID == -1)
    {
        response.send(Pistache::Http::Code::Internal_Server_Error, "Errore nel sistema di monitoraggio\n");
        return;
    }
    bool esito = rimuoviCorriere(trasporterID, ID);
    auto finish = std::chrono::high_resolution_clock::now(); // Memorizza il tempo di fine dell'operazione
    double elapsed = std::chrono::duration_cast<std::chrono::duration<double>> (finish-start).count();
    std::cout << elapsed << std::endl;
    if (elapsed > TEMPO_LIMITE) // Se il tempo dell'operazione è superiore al tempo limite, viene ritornato un timeout
    {
        response.send(Pistache::Http::Code::Internal_Server_Error, "La richiesta ha necessitato troppo tempo\n");
        fallimentoOperazione(logID);
        return;
    }
    if (esito) {
        response.send(Pistache::Http::Code::Ok, "Courier deleted from system\n");
        successoOperazione(logID);
    } else {
        response.send(Pistache::Http::Code::Unauthorized, "Failed to remove the courier from system\n");
        fallimentoOperazione(logID);
    }
}

// FUNZIONI AUSILIARIE
int recuperaCourierID(std::string IVA) 
{
    char comando[1000];
    PGresult *res;
    Con2DB db(HOSTNAME, DB_PORT, USERNAME_TRAS, PASSWORD_TRAS, DB_NAME); // Effettua la connessione al database
    int ID;
    sprintf(comando, "SELECT id FROM trasportatore WHERE piva = '%s' ", IVA.c_str());
    try
    {
        res = db.ExecSQLtuples(comando);
        ID = atoi(PQgetvalue(res, 0, PQfnumber(res, "id")));
    }
    catch(...)
    {
        std::cerr << "Errore nell'esecuzione della query!" << std::endl;
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
        sprintf(comando, "INSERT INTO cliente(Cliente_id, TipoUser, OperationType, Data_inizio) VALUES(%d, 'trasportatore', '%s', NOW()) RETURNING Id", customerID, operazione);
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

