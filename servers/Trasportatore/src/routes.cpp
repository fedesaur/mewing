#include "routes.h"

using json = nlohmann::json;

void defineRoutes(Pistache::Rest::Router& router) 
{
    // Registrazione delle rotte con funzioni globali
    Pistache::Rest::Routes::Get(router, "/autentica/:piva", Pistache::Rest::Routes::bind(&autenticaTrasportatore));
    Pistache::Rest::Routes::Get(router, "/:piva/ricerca/", Pistache::Rest::Routes::bind(&getOrdini));
    Pistache::Rest::Routes::Post(router, "/:piva/ordini/", Pistache::Rest::Routes::bind(&accettaOrdine));
    Pistache::Rest::Routes::Put(router, "/autentica/", Pistache::Rest::Routes::bind(&creaTrasportatore));
    

}

//curl -X GET http://localhost:5003/autentica/32132132132
void autenticaTrasportatore(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response)
{
    // Recupera l'email dal percorso
    std::string IVA = request.param(":piva").as<std::string>();

    if (IVA.empty()) {
        response.send(Pistache::Http::Code::Bad_Request, "IVA not provided");
        return;
    }
    // Ora chiama la funzione autentica
    int ID = autentica(IVA.c_str());

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
    
    if (esito) {
        response.send(Pistache::Http::Code::Created, "Trasporter created\n");
    } else {
        response.send(Pistache::Http::Code::Unauthorized, "Failed to create the Trasporter\n");
    }
}


//curl -X GET http://localhost:5003/32132132132/ricerca/
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
            
            */
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

//curl -X PUT -H "Content-Type: application/json" -d '{"ordine": 1, "corriere": 1}' http://localhost:5001/32132132132/ordini/
void accettaOrdine(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response)
{

    std::string IVA = request.param(":piva").as<std::string>();

    if (IVA.empty()) {
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
    
    bool esito = prendiOrdine(trasporterID, courierID, orderID);
    
    if (esito) {
        response.send(Pistache::Http::Code::Ok, "Ordine preso in carico\n");
    } else {
        response.send(Pistache::Http::Code::Internal_Server_Error, "Errore nel prendere in carico l'ordine\n (Un altro trasportatore potrebbe averlo già preso in carico)\n");
    }
}

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



