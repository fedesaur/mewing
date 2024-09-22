#include "routes.h"
using json = nlohmann::json; // Abbreviazione per il json

void defineRoutes(Pistache::Rest::Router& router) 
{
    // Registrazione delle rotte con funzioni globali
    Pistache::Rest::Routes::Get(router, "/autentica/:email", Pistache::Rest::Routes::bind(&autenticaFornitore));
    Pistache::Rest::Routes::Put(router, "/:email/prodotti/", Pistache::Rest::Routes::bind(&aggiungiProdotto));
    Pistache::Rest::Routes::Post(router, "/:email/prodotti/:idProdotto", Pistache::Rest::Routes::bind(&modificaProdotto));
    Pistache::Rest::Routes::Delete(router, "/:email/prodotti/:idProdotto", Pistache::Rest::Routes::bind(&eliminaProdotto));
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

//curl -X PUT -H "Content-Type: application/json" -d '{"nomeProdotto": "nome", "descrizioneProdotto": "descrizione", "prezzoProdotto": 1.23345}' http://localhost:5002/prova1@prova1.it/prodotti/
void modificaProdotto(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response)
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

    bool esito = modificaFornito(email.c_str(), nomeProdotto.c_str(), descrizioneProdotto.c_str(), prezzoProdotto);
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

    bool esito = modificaFornito(email.c_str(), nomeProdotto.c_str(), descrizioneProdotto.c_str(), prezzoProdotto, ID);
    if (esito) {
        response.send(Pistache::Http::Code::Created, "Product's attributes modified from system\n");
    } else {
        response.send(Pistache::Http::Code::Unauthorized, "Failed to change product's attributes\n");
    }
}
/*
void getOrdini(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response)
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
               ss << i + 1 << ") ID Ordine: " << ORDINI[i].ID
               << " Mail Customer: " << ORDINI[i].MailCustomer
               << " Data Richiesta: " << ORDINI[i].DataRichiesta
               << " Pagamento: " << ORDINI[i].Pagamento
               << " Totale: " << ORDINI[i].Totale << "\n"
               << " Da consegnare in: " << INDIRIZZI[i].via << " " << INDIRIZZI[i].civico
               << ", " << INDIRIZZI[i].citta << " (CAP : " << INDIRIZZI[i].CAP << "), "
               << INDIRIZZI[i].stato << "\n";
               /*
                
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
*/


