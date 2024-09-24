#include "routes.h"
using json = nlohmann::json; // Abbreviazione per il json

void defineRoutes(Pistache::Rest::Router& router) 
{
    // Registrazione delle rotte con funzioni globali
    Pistache::Rest::Routes::Get(router, "/:email/prodotti/", Pistache::Rest::Routes::bind(&getProdotti));
    Pistache::Rest::Routes::Get(router, "/autentica/:email", Pistache::Rest::Routes::bind(&autenticaFornitore));
    Pistache::Rest::Routes::Put(router, "/autentica/", Pistache::Rest::Routes::bind(&crea));
    Pistache::Rest::Routes::Put(router, "/:email/prodotti/", Pistache::Rest::Routes::bind(&aggiungiProdotto));
    Pistache::Rest::Routes::Post(router, "/:email/", Pistache::Rest::Routes::bind(&modificaInfo));
    Pistache::Rest::Routes::Post(router, "/:email/prodotti/:idProdotto", Pistache::Rest::Routes::bind(&modificaProdotto));
    Pistache::Rest::Routes::Delete(router, "/:email/prodotti/:idProdotto",   Pistache::Rest::Routes::bind(&eliminaProdotto));
}

int recuperaSupplierID(const std::string& email) 
{
    int ID;
    PGresult *res;
    char comando[1000];
    Con2DB db(HOSTNAME, DB_PORT, USERNAME, PASSWORD, DB_NAME); // Effettua la connessione al database

    // Prepara la query per cercare l'ID cliente tramite l'email
    sprintf(comando, "SELECT id FROM fornitore WHERE mail = '%s' ", email.c_str());
    
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

//curl -X GET http://localhost:5002/autentica/prova1@prova1.it
void autenticaFornitore(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response)
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
        response.send(Pistache::Http::Code::Ok, "Supplier authenticated\n");
    } else {
        response.send(Pistache::Http::Code::Unauthorized, "Authentication failed\n");
    }
}


//curl -X PUT -H "Content-Type: application/json" -d '{"nome": "nome", "IVA": "12312312312", "telefono": "1234567890", "email" : "simy@email.com", "via" : "Via lotteria", "civico": 12, "cap" : "12345", "city" : "Palermo", "stato" : "Repubblica delle Banane"}' http://localhost:5002/prova1@prova1.it/
void crea(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response)
{
    json dati = json::parse(request.body());
    // Controlla se i dati forniti dall'utente sono presenti e corretti
    if (!dati.contains("email") || dati["email"].empty()) response.send(Pistache::Http::Code::Bad_Request, "Email not provided\n");
    if (!dati.contains("nome") || dati["nome"].empty()) response.send(Pistache::Http::Code::Bad_Request, "Name not provided\n");
    if (!dati.contains("IVA") || dati["IVA"].empty()) response.send(Pistache::Http::Code::Bad_Request, "IVA not provided\n");
    if (!dati.contains("telefono") || dati["telefono"].empty()) response.send(Pistache::Http::Code::Bad_Request, "Telephone not provided\n");
    if (!dati.contains("via") || dati["via"].empty()) response.send(Pistache::Http::Code::Bad_Request, "Via not provided\n");
    if (!dati.contains("civico") || dati["civico"].empty()) response.send(Pistache::Http::Code::Bad_Request, "Civico not provided\n");
    if (!dati.contains("cap") || dati["cap"].empty()) response.send(Pistache::Http::Code::Bad_Request, "CAP not provided\n");
    if (!dati.contains("city") || dati["city"].empty()) response.send(Pistache::Http::Code::Bad_Request, "City not provided\n");
    if (!dati.contains("stato") || dati["stato"].empty()) response.send(Pistache::Http::Code::Bad_Request, "State not provided\n");
    
    std::string email = dati["email"];
    std::string nome = dati["nome"];
    std::string IVA = dati["IVA"];
    std::string telefono = dati["telefono"];
    std::string via = dati["via"];
    int civico = dati["civico"];
    std::string CAP = dati["cap"];
    std::string city = dati["city"];
    std::string stato = dati["stato"];

    if (email.length() > 50) response.send(Pistache::Http::Code::Bad_Request, "Mail length is above 50 characters\n");
    if (nome.length() > 100) response.send(Pistache::Http::Code::Bad_Request, "Name length is above 100 characters\n");
    if (IVA.length() != 11 || !isNumber(IVA) ) response.send(Pistache::Http::Code::Bad_Request, "IVA must be a string of 11 numbers\n");
    if (telefono.length() > 15 || telefono.length() < 10 || !isNumber(telefono)) response.send(Pistache::Http::Code::Bad_Request, "Telephone must be a string a 10-15 numbers\n");
    if (via.length() > 30) response.send(Pistache::Http::Code::Bad_Request, "Via length is above 50 characters\n");
    if (CAP.length() != 5 || !isNumber(CAP)) response.send(Pistache::Http::Code::Bad_Request, "CAP must be a string of 5 numbers\n");
    if (city.length() > 30) response.send(Pistache::Http::Code::Bad_Request, "City length is above 30 characters\n");
    if (stato.length() > 50) response.send(Pistache::Http::Code::Bad_Request, "State length is above 50 characters\n");
    
    //Chiama la funzione per creare il nuovo Fornitore
    bool esito = creaFornitore(email.c_str(), nome.c_str(), IVA.c_str(), telefono.c_str(), via.c_str(), civico, CAP.c_str(), city.c_str(), stato.c_str());
    if (esito) {
        response.send(Pistache::Http::Code::Created, "Supplier created\n");
    } else {
        response.send(Pistache::Http::Code::Unauthorized, "Failed to create the supplier\n");
    }
}

//curl -X PUT -H "Content-Type: application/json" -d '{"nomeProdotto": "nome", "descrizioneProdotto": "descrizione", "prezzoProdotto": 1.23345}' http://localhost:5002/prova1@prova1.it/prodotti/
void aggiungiProdotto(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response)
{
    // Recupera l'email del fornitore tra i parametri
    std::string email = request.param(":email").as<std::string>();
    json dati = json::parse(request.body());
    // Controlla se i dati forniti dall'utente sono presenti e corretti
    if (!dati.contains("nomeProdotto") || dati["nomeProdotto"].empty()) response.send(Pistache::Http::Code::Bad_Request, "Product name not provided\n");
    if (!dati.contains("descrizioneProdotto") || dati["descrizioneProdotto"].empty()) response.send(Pistache::Http::Code::Bad_Request, "Product description not provided\n");
    if (!dati.contains("prezzoProdotto") || dati["prezzoProdotto"].empty()) response.send(Pistache::Http::Code::Bad_Request, "Product price not provided\n");
    
    std::string nomeProdotto = dati["nomeProdotto"];
    std::string descrizioneProdotto = dati["descrizioneProdotto"];
    double prezzoProdotto = dati["prezzoProdotto"];

    bool esito = aggiungiFornito(email.c_str(), nomeProdotto.c_str(), descrizioneProdotto.c_str(), prezzoProdotto);
    if (esito) {
        response.send(Pistache::Http::Code::Created, "Product added to system\n");
    } else {
        response.send(Pistache::Http::Code::Unauthorized, "Failed to add the product to system\n");
    }
}

//curl -X DELETE http://localhost:5002/prova1@prova1.it/prodotti/1
void eliminaProdotto(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response)
{
    // Recupera l'email dal percorso
    std::string email = request.param(":email").as<std::string>();
    std::string id = request.param(":idProdotto").as<std::string>();

    // Controlla che i parametri richiesti siano stati forniti
    if (email.empty()) {
        response.send(Pistache::Http::Code::Bad_Request, "Email not provided\n");
        return;
    } else if (id.empty() ){
        response.send(Pistache::Http::Code::Bad_Request, "Product ID not provided\n");
        return;
    }
    int ID = stoi(id);
    bool esito = rimuoviFornito(email.c_str(), ID); // Ora chiama la funzione autentica
    if (esito) {
        response.send(Pistache::Http::Code::Ok, "Product deleted from system\n");
    } else {
        response.send(Pistache::Http::Code::Unauthorized, "Failed to remove the product from system\n");
    }
}

//curl -X POST -H "Content-Type: application/json" -d '{"nomeProdotto": "nome", "descrizioneProdotto": "descrizione", "prezzoProdotto": 1.23345}' http://localhost:5002/prova1@prova1.it/prodotti/1
void modificaProdotto(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response)
{
    // Recupera l'email del fornitore tra i parametri
    std::string email = request.param(":email").as<std::string>();
    std::string id = request.param(":idProdotto").as<std::string>(); 

    // Controlla che i parametri richiesti siano stati forniti
    if (email.empty()) {
        response.send(Pistache::Http::Code::Bad_Request, "Email not provided\n");
        return;
    } else if (id.empty() ){
        response.send(Pistache::Http::Code::Bad_Request, "Product ID not provided\n");
        return;
    }

    json dati = json::parse(request.body());
    // Controlla se i dati forniti dall'utente sono presenti e corretti
    if (!dati.contains("nomeProdotto") || dati["nomeProdotto"].empty()) response.send(Pistache::Http::Code::Bad_Request, "Product name not provided\n");
    if (!dati.contains("descrizioneProdotto") || dati["descrizioneProdotto"].empty()) response.send(Pistache::Http::Code::Bad_Request, "Product description not provided\n");
    if (!dati.contains("prezzoProdotto") || dati["prezzoProdotto"].empty()) response.send(Pistache::Http::Code::Bad_Request, "Product price not provided\n");
    
    std::string nomeProdotto = dati["nomeProdotto"];
    std::string descrizioneProdotto = dati["descrizioneProdotto"];
    double prezzoProdotto = dati["prezzoProdotto"];
    int ID = stoi(id);
    
    int supID = recuperaSupplierID(email);
    
    // Connetti a Redis
    redisContext *c2r = redisConnect(REDIS_IP, REDIS_PORT);
    if (c2r == nullptr || c2r->err) {
        response.send(Pistache::Http::Code::Internal_Server_Error, "Unable to connect to Redis");
        return;
    }

    // Prepara il comando per aggiungere l'email allo stream
    redisReply* reply = static_cast<redisReply*>(redisCommand(c2r, "XADD %s * id %d", WRITE_STREAM, supID));

    // Controlla l'esito del comando Redis
    if (reply == nullptr || reply->type != REDIS_REPLY_STRING) {
        response.send(Pistache::Http::Code::Internal_Server_Error, "Error writing id to Redis stream");
        redisFree(c2r);
        return;
    }

    // Pulisci la risposta di Redis
    freeReplyObject(reply);
    redisFree(c2r);

    bool esito = modificaFornito(email.c_str(), nomeProdotto.c_str(), descrizioneProdotto.c_str(), prezzoProdotto, ID);
    if (esito) {
        response.send(Pistache::Http::Code::Created, "Product's attributes modified from system\n");
    } else {
        response.send(Pistache::Http::Code::Unauthorized, "Failed to change product's attributes\n");
    }
}

//curl -X GET http://localhost:5002/prova1@prova1.it/prodotti/
void getProdotti(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response)
{
    std::stringstream ss;
    int RIGHE;

    std::string email = request.param(":email").as<std::string>();
    
    // Controlla che i parametri richiesti siano stati forniti
    if (email.empty()) {
        response.send(Pistache::Http::Code::Bad_Request, "Email not provided\n");
        return;
    }

    // Connessione a Redis
    redisContext *redis = redisConnect("127.0.0.1", 6379);  // Redis su localhost
    if (redis == nullptr || redis->err) {
        std::cerr << "Errore nella connessione a Redis" << std::endl;
        response.send(Pistache::Http::Code::Internal_Server_Error, "Failed to connect to Redis");
        return;
    }

    // Recupera la lista di ID prodotti per l'email dal Redis
    redisReply* reply = (redisReply*)redisCommand(redis, "LRANGE prodotti:%s 0 -1", email.c_str());
    if (reply->type == REDIS_REPLY_ARRAY && reply->elements > 0) {
        RIGHE = reply->elements;
        Prodotto* forniti = new Prodotto[RIGHE];  // Array dinamico di prodotti

        // Recupera i prodotti da Redis
        for (int i = 0; i < RIGHE; i++) {
            std::string prodottoID = reply->element[i]->str;

            // Recupera il prodotto come hash da Redis
            redisReply* prodottoReply = (redisReply*)redisCommand(redis, "HGETALL prodotto:%s", prodottoID.c_str());
            if (prodottoReply->type == REDIS_REPLY_ARRAY && prodottoReply->elements == 10) {  // Assicurati di avere tutti i campi
                forniti[i].ID = std::stoi(prodottoReply->element[1]->str);

                // Copia i valori delle stringhe in buffer di memoria allocati dinamicamente
                // Nome
                std::string nomeTemp = prodottoReply->element[3]->str;
                forniti[i].nome = new char[nomeTemp.length() + 1];
                std::strcpy(const_cast<char*>(forniti[i].nome), nomeTemp.c_str());

                // Descrizione
                std::string descrizioneTemp = prodottoReply->element[5]->str;
                forniti[i].descrizione = new char[descrizioneTemp.length() + 1];
                std::strcpy(const_cast<char*>(forniti[i].descrizione), descrizioneTemp.c_str());

                // Fornitore
                std::string fornitoreTemp = prodottoReply->element[7]->str;
                forniti[i].fornitore = new char[fornitoreTemp.length() + 1];
                std::strcpy(const_cast<char*>(forniti[i].fornitore), fornitoreTemp.c_str());

                // Prezzo
                forniti[i].prezzo = std::stod(prodottoReply->element[9]->str);

            } else {
                response.send(Pistache::Http::Code::Internal_Server_Error, "Errore nel recupero di un prodotto da Redis");
                freeReplyObject(prodottoReply);
                redisFree(redis);
                return;
            }
            freeReplyObject(prodottoReply);
        }

        // Stampa i prodotti
        ss << "\nPRODOTTI FORNITI:\n";
        for (int i = 0; i < RIGHE; i++) {
            ss << i + 1 << ") ID Prodotto: " << forniti[i].ID
               << " Nome Prodotto: " << forniti[i].nome
               << " Descrizione: " << forniti[i].descrizione
               << " Fornitore: " << forniti[i].fornitore
               << " Prezzo Prodotto: " << forniti[i].prezzo
               <<  "\n";
        }

        // Invia la risposta con i prodotti
        response.send(Pistache::Http::Code::Ok, ss.str());

        // Libera la memoria allocata dinamicamente
        for (int i = 0; i < RIGHE; i++) {
            delete[] forniti[i].nome;
            delete[] forniti[i].descrizione;
            delete[] forniti[i].fornitore;
        }
        delete[] forniti;
    } else {
        // Nessun prodotto trovato in Redis
        response.send(Pistache::Http::Code::Ok, "Nessun prodotto disponibile in Redis");
    }

    freeReplyObject(reply);
    redisFree(redis);  // Chiudi la connessione a Redis
}


//curl -X POST -H "Content-Type: application/json" -d '{"nome": "nome", "IVA": "12312312312", "telefono": "1234567890"}' http://localhost:5002/prova1@prova1.it/
void modificaInfo(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response)
{
    // Recupera l'email del fornitore tra i parametri
    std::string email = request.param(":email").as<std::string>();
    json dati = json::parse(request.body());
    // Controlla se i dati forniti dall'utente sono presenti e corretti
    if (!dati.contains("nome") || dati["nome"].empty()) response.send(Pistache::Http::Code::Bad_Request, "Name not provided\n");
    if (!dati.contains("IVA") || dati["IVA"].empty()) response.send(Pistache::Http::Code::Bad_Request, "IVA not provided\n");
    if (!dati.contains("telefono") || dati["telefono"].empty()) response.send(Pistache::Http::Code::Bad_Request, "Telephone not provided\n");
    
    std::string nome = dati["nome"];
    std::string IVA = dati["IVA"];
    std::string telefono = dati["telefono"];
    if (nome.length() > 100) response.send(Pistache::Http::Code::Bad_Request, "Name length is above 100 characters\n");
    if (IVA.length() != 11 || !isNumber(IVA) ) response.send(Pistache::Http::Code::Bad_Request, "IVA must be a string of 11 numbers\n");
    if (telefono.length() > 15 || telefono.length() < 10 || !isNumber(telefono)) response.send(Pistache::Http::Code::Bad_Request, "Telephone must be a string a 10-15 numbers\n");

    bool esito = modificaInfoF(email.c_str(), nome.c_str(), IVA.c_str(), telefono.c_str());
    if (esito) {
        response.send(Pistache::Http::Code::Created, "Info changes\n");
    } else {
        response.send(Pistache::Http::Code::Unauthorized, "Failed to change your info\n");
    }
}


