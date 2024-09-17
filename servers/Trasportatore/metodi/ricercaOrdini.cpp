#include "ricercaOrdini.h"

std::pair<int, Ordine*> ricercaOrdini(int clientSocket)
{
    int COURIER_ID;
    int rows;
    char comando[1000];
    std::pair<int, Ordine*> risultato;
    PGresult *res;
    redisContext *c2r; // c2r contiene le info sul contesto
	redisReply *reply; // reply contiene le risposte da Redis

	c2r = redisConnect(REDIS_IP, REDIS_PORT); // Effettua la connessione a Redis
	Con2DB db(HOSTNAME, DB_PORT, USERNAME_TRAS, PASSWORD_TRAS, DB_NAME); // Effettua la connessione al database

    reply = RedisCommand(c2r, "XREVRANGE %s + - COUNT 1", READ_STREAM);
    if (reply == nullptr || reply->type != REDIS_REPLY_ARRAY || reply->elements == 0)
    {
        std::cerr << "Errore nel comando Redis o stream vuoto" << std::endl;
    }

    std::string id = reply->element[0]->element[1]->element[1]->str; 
    COURIER_ID = atoi(id.c_str()); // ID Customer
    sprintf(comando, "SELECT ord.id, cst.nome, ord.datarich, ord.stato, ord.pagamento, ord.indirizzo, ord.totale "
    "FROM ordine ord, customers cst WHERE cst.id = ord.customer AND ord.id NOT IN (SELECT id FROM ordineconse) AND ord.id NOT IN (SELECT ordine FROM consegna) "
    "ORDER BY ord.datarich");
    try
    {
        //Recupera tutti gli ordini disponibili (non ancora cons)
        res = db.ExecSQLtuples(comando);
        rows = PQntuples(res);
        if (rows > 0)
        {
            Ordine* ordiniDisponibili = new Ordine[rows];
            for (int i = 0; i < rows; i++)
            {
                // Recupera gli attributi dei prodotti dalla query sopra svolta...
                int ID = atoi(PQgetvalue(res, i, PQfnumber(res, "id")));
                const char* mail = PQgetvalue(res, i, PQfnumber(res, "customer"));
                unsigned char* data = (unsigned char*) PQgetvalue(res, i, PQfnumber(res, "datarich"));
                time_t time = static_cast<time_t>(std::stoll(reinterpret_cast<char*>(data))); // Converte il timestamp in time_t
                double prezzo = atof(PQgetvalue(res, i, PQfnumber(res, "prezzo")));
                const char* pagamento = PQgetvalue(res, i, PQfnumber(res, "pagamento"));;
                int indirizzo = atoi(PQgetvalue(res, i, PQfnumber(res, "indirizzo")));

            // Assegna gli attributi all'i-esimo Prodotto in prodottiDisponibili
                ordiniDisponibili[i].ID = ID;
                ordiniDisponibili[i].MailCustomer = mail;
                ordiniDisponibili[i].DataRichiesta = time;
                ordiniDisponibili[i].Pagamento = pagamento;
                ordiniDisponibili[i].Indirizzo = indirizzo;
                ordiniDisponibili[i].Totale = prezzo;
            }
            risultato.first = rows;
            risultato.second = ordiniDisponibili;
        }
        else
        {
            risultato.first = 0;
            risultato.second = nullptr;
        }
    }
    catch(...)
    {
        std::string errore = "C'è stato un problema con il database\n";
        send(clientSocket, errore.c_str(), errore.length(), 0);
        risultato.first = -1;
        risultato.second = nullptr;
    }
    return risultato;
}

void mostraOrdini(int clientSocket, int RIGHE, Ordine* ORDINI)
{
    if (RIGHE > 0)
    {
        std::string request = "\nORDINI DISPONIBILI:\n"; //... e lo stampa
	    send(clientSocket, request.c_str(), request.length(), 0); // Invia il messaggio pre-impostato all'utente
        for (int i = 0; i < RIGHE; i++)
        {
            // Recupera gli attributi degli ordini disponibili...
            int ID = ORDINI[i].ID;
            const char* mail = ORDINI[i].MailCustomer;
            time_t data = ORDINI[i].DataRichiesta;
            const char* paga = ORDINI[i].Pagamento;
            int indirizzo =ORDINI[i].Indirizzo;
            double totale = ORDINI[i].Totale;
            // ...e li invia all'utente così che possa visualizzarli ed effettuarci operazioni
            std::string ordine = std::to_string(i+1) + ") ID Ordine: " + std::to_string(ID) +
             " Mail Customer: " + mail + 
             " Data Richiesta: " + std::to_string(data) + 
             " Metodo Pagamento: " + paga +
             " Totale Ordine: " + std::to_string(totale) + "\n";
	        send(clientSocket, ordine.c_str(), ordine.length(), 0);
        }
    } else {
        std::string vuoto = "Non ci sono ordini disponibili!\n";
        send(clientSocket, vuoto.c_str(), vuoto.length(), 0); // Invia la frase all'utente
    }
    return;
}


       
            
