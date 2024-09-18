#include "registroOrdini.h"

std::tuple<int, Ordine*, Indirizzo*> registroOrdini(int clientSocket)  
//per il momento metto optional per ritornare una tupla vuota
{
    int COURIER_ID;
    int rows;
    char comando[1000];
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
    COURIER_ID = atoi(id.c_str()); // ID Corriere
    sprintf(comando, "SELECT ord.id, ord.datarich, ord.stato, ord.pagamento, ind.via, ind.civico, ind.cap, ind.città, ind.stato AS statoIND, ord.totale"
    "FROM indirizzo ind, customers cst, ordine ord LEFT JOIN transord tr ON ord.id = tr.ordine"
    "WHERE ind.id = ord.indirizzo AND ord.customer = cst.id AND tr.trasportatore = %d "
    "ORDER BY ord.datarich", COURIER_ID);
    try
    {
        //Recupera tutti gli ordini disponibili (non ancora cons)
        res = db.ExecSQLtuples(comando);
        rows = PQntuples(res);
        if (rows > 0)
        {
            Ordine* ordiniDisponibili = new Ordine[rows];
            Indirizzo* indirizzoOrdini = new Indirizzo[rows];
            for (int i = 0; i < rows; i++)
            {
                // Recupera gli attributi degli ordini e degli indirizzi della query sopra svolta...
                int ID = atoi(PQgetvalue(res, i, PQfnumber(res, "id")));
                const char* mail = PQgetvalue(res, i, PQfnumber(res, "nome"));
                unsigned char* data = (unsigned char*) PQgetvalue(res, i, PQfnumber(res, "datarich"));
                time_t time = static_cast<time_t>(std::stoll(reinterpret_cast<char*>(data))); // Converte il timestamp in time_t
                const char* statoOrd = atof(PQgetvalue(res, i, PQfnumber(res, "stato")));
                const char* paga = PQgetvalue(res, i, PQfnumber(res, "pagamento"));
                const char* via = PQgetvalue(res, i, PQfnumber(res, "via"));
                int civico = atoi(PQgetvalue(res, i, PQfnumber(res, "civico")));
                const char* CAP = PQgetvalue(res, i, PQfnumber(res, "cap"));
                const char* city = PQgetvalue(res, i, PQfnumber(res, "citta"));
                const char* stato = PQgetvalue(res, i, PQfnumber(res, "statoIND"));
                double totale = atof(PQgetvalue(res, i, PQfnumber(res, "totale")));

                //...e li assegna all'i-esimo Ordine e all'i-esimo Indirizzo
                ordiniDisponibili[i].ID = ID;
                ordiniDisponibili[i].MailCustomer = mail;
                ordiniDisponibili[i].DataRichiesta = time;
                ordiniDisponibili[i].Stato = statoOrd;
                ordiniDisponibili[i].Pagamento = paga;
                ordiniDisponibili[i].Totale = totale;

                indirizzoOrdini[i].via = via;
                indirizzoOrdini[i].civico = civico;
                indirizzoOrdini[i].CAP = CAP;
                indirizzoOrdini[i].citta = city;
                indirizzoOrdini[i].stato = stato;
            }
            ordinaOrdini(rows, ordiniDisponibili, indirizzoOrdini);
            std::tuple<int, Ordine*, Indirizzo*> risultato(rows, ordiniDisponibili, indirizzoOrdini);
            return risultato;
        }
        else
        {
            std::string vuoto = "Nessun ordine registrato!\n";
            send(clientSocket, vuoto.c_str(), vuoto.length(), 0);
            std::tuple<int, Ordine*, Indirizzo*> vuoto(0,nullptr, nullptr);
            return vuoto;
        }
    }
    catch(...)
    {
        std::string errore = "C'è stato un problema con il database\n";
        send(clientSocket, errore.c_str(), errore.length(), 0);
        std::tuple<int, Ordine*, Indirizzo*> ritorno(-1, nullptr, nullptr);
        return ritorno;
    }
}

void mostraRegistro(int clientSocket, int RIGHE, Ordine* ORDINI, Indirizzo* INDIRIZZI)
{
    if (RIGHE > 0)
    {
        std::string request = "\nORDINI REGISTRATI:\n"; //... e lo stampa
	    send(clientSocket, request.c_str(), request.length(), 0); // Invia il messaggio pre-impostato all'utente
        for (int i = 0; i < RIGHE; i++)
        {
            // Recupera gli attributi degli ordini registrati...
            int ID = ORDINI[i].ID;
                ordiniDisponibili[i].MailCustomer = mail;
                ordiniDisponibili[i].DataRichiesta = time;
                ordiniDisponibili[i].Stato = statoOrd;
                ordiniDisponibili[i].Pagamento = paga;
                ordiniDisponibili[i].Totale = totale;

                indirizzoOrdini[i].via = via;
                indirizzoOrdini[i].civico = civico;
                indirizzoOrdini[i].CAP = CAP;
                indirizzoOrdini[i].citta = city;
                indirizzoOrdini[i].stato = stato;
            // ...e li invia all'utente così che possa visualizzarli ed effettuarci operazioni
            std::string ordine = std::to_string(i+1) + ") ID Ordine: " + std::to_string(ID) +
             " Mail Customer: " + mail + 
             " Data Richiesta: " + std::to_string(data) + 
             " Metodo Pagamento: " + paga +
             " Totale Ordine: " + std::to_string(totale) + "\n";
	        send(clientSocket, ordine.c_str(), ordine.length(), 0);
        }
    } else {
        std::string vuoto = "Non ci sono ordini registrati!\n";
        send(clientSocket, vuoto.c_str(), vuoto.length(), 0); // Invia la frase all'utente
    }
    return;
}