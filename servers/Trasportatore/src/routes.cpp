#include "routes.h"

void defineRoutes(Pistache::Rest::Router& router) 
{
    // Registrazione delle rotte con funzioni globali
    Pistache::Rest::Routes::Post(router, "/autentica/:piva", Pistache::Rest::Routes::bind(&autenticaTrasportatore));
    Pistache::Rest::Routes::Get(router, "/ordini/", Pistache::Rest::Routes::bind(&getOrdiniDisponibili));
    Pistache::Rest::Routes::Post(router, "/addToCarrello/:email/:prodotto/:quantita", Pistache::Rest::Routes::bind(&addProdottoToCarrello));
    Pistache::Rest::Routes::Get(router, "/carrello/:email", Pistache::Rest::Routes::bind(&getCarrello));
    Pistache::Rest::Routes::Post(router, "/ordina/:email", Pistache::Rest::Routes::bind(&ordina));

}


void autenticaTrasportatore(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response)
{
    // Recupera l'email dal percorso
    std::string IVA = request.param(":piva").as<std::string>();

    if (IVA.empty()) {
        response.send(Pistache::Http::Code::Bad_Request, "IVA not provided");
        return;
    }


    // Connetti a Redis
    redisContext *c2r = redisConnect(REDIS_IP, REDIS_PORT);
    if (c2r == nullptr || c2r->err) {
        response.send(Pistache::Http::Code::Internal_Server_Error, "Unable to connect to Redis");
        return;
    }

    // Prepara il comando per aggiungere l'email allo stream
    redisReply* reply = static_cast<redisReply*>(redisCommand(c2r, "XADD %s * iva %s", WRITE_STREAM, IVA.c_str()));

    // Controlla l'esito del comando Redis
    if (reply == nullptr || reply->type != REDIS_REPLY_STRING) 
    {
        response.send(Pistache::Http::Code::Internal_Server_Error, "Error writing iva to Redis stream");
        redisFree(c2r);
        return;
    }

    // Pulisci la risposta di Redis
    freeReplyObject(reply);
    redisFree(c2r);

    // Ora chiama la funzione autentica
    bool autenticato = autentica();
    int ID = recuperaTrasporterID(IVA);

    if (autenticato && ID > 0) {
        response.send(Pistache::Http::Code::Ok, "Trasporter authenticated");
        reply = RedisCommand(c2r, "XADD %s * id %d", WRITE_STREAM, ID);
        assertReplyType(c2r, reply, REDIS_REPLY_STRING);
        freeReplyObject(reply);
        // Mette l'ID del trasportatore sullo stream in modo che possa essere reperito poi
    } else {
        response.send(Pistache::Http::Code::Unauthorized, "Authentication failed");
    }
}

int recuperaTrasporterID(std::string IVA) 
{
    char comando[1000];
    PGresult *res;
    Con2DB db(HOSTNAME, DB_PORT, USERNAME_TRAS, PASSWORD_TRAS, DB_NAME); // Effettua la connessione al database
    int ID;
    sprintf(comando, "SELECT id FROM trasporatore WHERE piva = '%s' ", IVA.c_str());
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

void getOrdiniDisponibili(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response)
{
    int rows;
    char comando[1000];
    PGresult *res;
    Con2DB db(HOSTNAME, DB_PORT, USERNAME_TRAS, PASSWORD_TRAS, DB_NAME); // Effettua la connessione al database
    // Recupera tutti gli ordini disponibili
    
    std::tuple<int, Ordine*, Indirizzo*> risultato = ricercaOrdini();
    if (std::get<0>(risultato) == -1) response.send(Pistache::Http::Code::Internal_Server_Error, "Errore nel recupero degli ordini\n");
    
    int RIGHE = std::get<0>(risultato);
    Ordine* ORDINI = std::get<1>(risultato);
    Indirizzo* INDIRIZZI =  std::get<2>(risultato);
    // Costruisci la risposta
    try
    {
        if (RIGHE > 0) 
        {
            std::stringstream ss;
            ss << "ORDINI DISPONIBILI:\n";

            // Itera sui prodotti e li inserisce nella stringa di risposta
            for (int i = 0; i < RIGHE; i++) 
            {
                ss << i + 1 << ") ID Ordine: " << std::to_string(ORDINI[i].ID)
               << " Mail Customer: " << ORDINI[i].MailCustomer
               << " Data Richiesta: " << std::to_string(ORDINI[i].DataRichiesta)
               << " Pagamento: " << ORDINI[i].Pagamento
               << " Totale: " << std::to_string(ORDINI[i].Totale) << "\n"
               << " Da consegnare in: " << INDIRIZZI[i].via << " " << std::to_string(INDIRIZZI[i].civico)
               << ", " + INDIRIZZI[i].citta << " (CAP : " << INDIRIZZI[i].CAP + "), "
               << INDIRIZZI[i].stato + "\n";

                sprintf(comando, "SELECT pr.id, pr.descrizione, pr.nome, pr.prezzo, fr.nome AS nomeF, pn.quantita "
                "FROM prodotto pr, prodinord pn, fornitore fr WHERE pn.prodotto = pr.id "
                "AND pr.fornitore = fr.id AND pn.ordine = %d", ORDINI[i].ID);
                res = db.ExecSQLtuples(comando);
                rows = PQntuples(res); // Si presume ci siano prodotti nell'ordine
                ss << "PRODOTTI PRESENTI NELL'ORDINE:\n";
                for (int j = 0; j < rows; j++)
                {
                    int IDProd = atoi(PQgetvalue(res, j, PQfnumber(res, "id")));
                    const char* descrizione = PQgetvalue(res, j, PQfnumber(res, "descrizione"));
                    double prezzo = atof(PQgetvalue(res, j, PQfnumber(res, "prezzo")));
                    const char* nome = PQgetvalue(res, j, PQfnumber(res, "nome"));
                    const char* fornitore = PQgetvalue(res, j, PQfnumber(res, "nomeF"));
                    int quantita = atoi(PQgetvalue(res, j, PQfnumber(res, "quantita")));

                    ss << "\t" << std::to_string(j+1) + ") ID Prodotto: " << std::to_string(IDProd)
                    << " Nome Prodotto: " << nome
                    << " Descrizione: " << descrizione
                    << " Fornitore: " << fornitore
                    << " Prezzo Prodotto: " << std::to_string(prezzo)
                    << " Quantità :" << std::to_string(quantita) << "\n";
                }
                PQclear(res);
            }
        // Pulisci la memoria allocata dinamicamente per gli ordini
        delete[] ORDINI;
        delete[] INDIRIZZI;

        response.send(Pistache::Http::Code::Ok, ss.str());
        } else if (RIGHE == 0) {
            response.send(Pistache::Http::Code::Ok, "Nessun ordine disponibile");
        } else {
            response.send(Pistache::Http::Code::Internal_Server_Error, "Errore nel recupero dei prodotti");
        }
    }
    catch(...)
    {
        response.send(Pistache::Http::Code::Internal_Server_Error, "Errore nel recupero dei prodotti");
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
