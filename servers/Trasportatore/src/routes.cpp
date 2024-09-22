#include "routes.h"

void defineRoutes(Pistache::Rest::Router& router) 
{
    // Registrazione delle rotte con funzioni globali
    Pistache::Rest::Routes::Post(router, "/autentica/:piva", Pistache::Rest::Routes::bind(&autenticaTrasportatore));
    Pistache::Rest::Routes::Get(router, "/ordini/", Pistache::Rest::Routes::bind(&getOrdini));
    

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
        //reply = RedisCommand(c2r, "XADD %s * id %d", WRITE_STREAM, ID);
        //assertReplyType(c2r, reply, REDIS_REPLY_STRING);
        //freeReplyObject(reply);
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



