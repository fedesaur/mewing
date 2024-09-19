#include "registroOrdini.h"

std::tuple<int, Ordine*, Corriere*> registroOrdini(int clientSocket)  
{
    int COURIER_ID;
    char comando[1000];
    int RIGHE_CORRENTI;
    int RIGHE_CONSEGNATI;
    int RIGHE_TOTALI;
    Ordine* ORDINI_INCORSO;
    Ordine* ORDINI_TOTALI;
    Corriere* CORRIERI_INCORSO;
    Corriere* CORRIERI_TOTALI;
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
    try
    {
        // Recupera gli ordini in corso e non ancora consegnati
        sprintf(comando, "SELECT ord.id, cst.mail, ord.datarich, ord.stato, ord.pagamento, ord.totale, cor.id AS CorID, cor.nome, cor.cognome"
        "FROM customers cst, ordine ord, consegna cons, corriere cor, WHERE ord.id = cons.ordine "
        "AND cor.id = cons.corriere AND ord.customer = cst.id AND cor.azienda = %d AND ord.id NOT IN (SELECT id FROM ordineconse) "
        "ORDER BY ord.datarich", COURIER_ID);
        res = db.ExecSQLtuples(comando);
        RIGHE_CORRENTI = PQntuples(res);
        if (RIGHE_CORRENTI > 0)
        {
            ORDINI_INCORSO = new Ordine[RIGHE_CORRENTI];
            CORRIERI_INCORSO = new Corriere[RIGHE_CORRENTI];
            for (int i = 0; i < RIGHE_CORRENTI; i++)
            {
                // Recupera gli attributi degli ordini e i corrieri della query sopra svolta...
                int ID = atoi(PQgetvalue(res, i, PQfnumber(res, "id")));
                const char* mail = PQgetvalue(res, i, PQfnumber(res, "mail"));
                unsigned char* data = (unsigned char*) PQgetvalue(res, i, PQfnumber(res, "datarich"));
                time_t time = static_cast<time_t>(std::stoll(reinterpret_cast<char*>(data))); // Converte il timestamp in time_t
                const char* statoOrd = PQgetvalue(res, i, PQfnumber(res, "stato"));
                const char* paga = PQgetvalue(res, i, PQfnumber(res, "pagamento"));
                double totale = atof(PQgetvalue(res, i, PQfnumber(res, "totale")));
                int IDCor = atoi(PQgetvalue(res, i, PQfnumber(res, "CorID")));
                const char* nome = PQgetvalue(res, i, PQfnumber(res, "nome"));
                const char* cognome = PQgetvalue(res, i, PQfnumber(res, "cognome"));

                //...e li assegna all'i-esimo Ordine...
                ORDINI_INCORSO[i].ID = ID;
                ORDINI_INCORSO[i].MailCustomer = mail;
                ORDINI_INCORSO[i].DataRichiesta = time;
                ORDINI_INCORSO[i].Stato = statoOrd;
                ORDINI_INCORSO[i].Pagamento = paga;
                ORDINI_INCORSO[i].Totale = totale;

                //...e all'i-esimo Corriere (ci saranno duplicati)
                CORRIERI_INCORSO[i].ID = IDCor;
                CORRIERI_INCORSO[i].nome = nome;
                CORRIERI_INCORSO[i].cognome = cognome;
            }
        }
        PQclear(res);
        // Recupera gli ordini consegnati
        sprintf(comando, "SELECT ord.id, cst.mail, ord.datarich, orc.datacons, ord.stato, ord.pagamento, ord.totale, cor.id AS CorID, cor.nome, cor.cognome"
        "FROM customers cst, ordine ord, consegna cons, corriere cor, ordineconse orc WHERE ord.id = cons.ordine "
        "AND cor.id = cons.corriere AND ord.customer = cst.id AND cor.azienda = %d AND ord.id = orc.id "
        "ORDER BY orc.datacons", COURIER_ID);
        res = db.ExecSQLtuples(comando);
        RIGHE_CONSEGNATI = PQntuples(res);
        RIGHE_TOTALI = RIGHE_CORRENTI + RIGHE_CONSEGNATI;
        if ((RIGHE_CONSEGNATI + RIGHE_CORRENTI) == 0)
        {
            std::tuple<int, Ordine*, Corriere*> risultato(0, nullptr, nullptr);
            return risultato;
        }

        ORDINI_TOTALI = new Ordine[RIGHE_TOTALI];
        CORRIERI_TOTALI = new Corriere[RIGHE_TOTALI];
        for (int i = 0; i < RIGHE_CORRENTI; i++) // Copia gli ordini in corso nell'insieme finale
        {
                ORDINI_TOTALI[i].ID = ORDINI_INCORSO[i].ID;
                ORDINI_TOTALI[i].MailCustomer = ORDINI_INCORSO[i].MailCustomer;
                ORDINI_TOTALI[i].DataRichiesta = ORDINI_INCORSO[i].DataRichiesta;
                ORDINI_TOTALI[i].DataConsegna = NULL;
                ORDINI_TOTALI[i].Stato = ORDINI_INCORSO[i].Stato;
                ORDINI_TOTALI[i].Pagamento = ORDINI_INCORSO[i].Pagamento;
                ORDINI_TOTALI[i].Totale = ORDINI_INCORSO[i].Totale;

                CORRIERI_TOTALI[i].ID = CORRIERI_INCORSO[i].ID;
                CORRIERI_TOTALI[i].nome = CORRIERI_INCORSO[i].nome;
                CORRIERI_TOTALI[i].cognome = CORRIERI_INCORSO[i].cognome;
        }

        for (int i = 0; i < RIGHE_CONSEGNATI; i++) // Dopodichè copia gli ordini consegnati al suo interno
        {
            // Recupera gli attributi degli ordini e i corrieri della query sopra svolta...
            int ID = atoi(PQgetvalue(res, i, PQfnumber(res, "id")));
            const char* mail = PQgetvalue(res, i, PQfnumber(res, "mail"));
            unsigned char* data1 = (unsigned char*) PQgetvalue(res, i, PQfnumber(res, "datarich"));
            time_t time = static_cast<time_t>(std::stoll(reinterpret_cast<char*>(data1))); // Converte il timestamp in time_t
            unsigned char* data2 = (unsigned char*) PQgetvalue(res, i, PQfnumber(res, "datarich"));
            time_t timeCons = static_cast<time_t>(std::stoll(reinterpret_cast<char*>(data2))); // Converte il timestamp in time_t
            const char* statoOrd = PQgetvalue(res, i, PQfnumber(res, "stato"));
            const char* paga = PQgetvalue(res, i, PQfnumber(res, "pagamento"));
            double totale = atof(PQgetvalue(res, i, PQfnumber(res, "totale")));
            int IDCor = atoi(PQgetvalue(res, i, PQfnumber(res, "CorID")));
            const char* nome = PQgetvalue(res, i, PQfnumber(res, "nome"));
            const char* cognome = PQgetvalue(res, i, PQfnumber(res, "cognome"));

            //...e li assegna all'i-esimo Ordine...
            ORDINI_TOTALI[RIGHE_CORRENTI + i].ID = ID;
            ORDINI_TOTALI[RIGHE_CORRENTI + i].MailCustomer = mail;
            ORDINI_TOTALI[RIGHE_CORRENTI + i].DataRichiesta = time;
            ORDINI_TOTALI[RIGHE_CORRENTI + i].DataConsegna = timeCons;
            ORDINI_TOTALI[RIGHE_CORRENTI + i].Stato = statoOrd;
            ORDINI_TOTALI[RIGHE_CORRENTI + i].Pagamento = paga;
            ORDINI_TOTALI[RIGHE_CORRENTI + i].Totale = totale;

            //...e all'i-esimo Corriere
            CORRIERI_TOTALI[RIGHE_CORRENTI + i].ID = IDCor;
            CORRIERI_TOTALI[RIGHE_CORRENTI + i].nome = nome;
            CORRIERI_TOTALI[RIGHE_CORRENTI + i].cognome = cognome;
        }
        delete[] ORDINI_INCORSO;
        delete[] CORRIERI_INCORSO;
        std::tuple<int, Ordine*, Indirizzo*> ritorno(RIGHE_TOTALI, ORDINI_TOTALI, CORRIERI_TOTALI);
        return ritorno;
    }
    catch(...)
    {
        std::string errore = "C'è stato un problema con il database\n";
        send(clientSocket, errore.c_str(), errore.length(), 0);
        std::tuple<int, Ordine*, Indirizzo*> ritorno(-1, nullptr, nullptr);
        return ritorno;
    }
}

void mostraRegistro(int clientSocket, int RIGHE, Ordine* ORDINI, Corriere* CORRIERI)
{
    if (RIGHE > 0)
    {
        std::string request = "\nORDINI REGISTRATI:\n"; //... e lo stampa
	    send(clientSocket, request.c_str(), request.length(), 0); // Invia il messaggio pre-impostato all'utente
        for (int i = 0; i < RIGHE; i++)
        {
            // Recupera gli attributi degli ordini registrati e li invia all'utente così che possa visualizzarli ed effettuarci operazioni
            std::string ordine = std::to_string(i+1) + ") ID Ordine: " + std::to_string(ORDINI[i].ID) +
             " Mail Customer: " + ORDINI[i].MailCustomer + 
             " Data Richiesta: " + std::to_string(ORDINI[i].DataRichiesta) + 
             " Stato Ordine: " + ORDINI[i].Stato +
             " Metodo Pagamento: " + ORDINI[i].Pagamento +
             " Totale Ordine: " + std::to_string(ORDINI[i].Totale) + "\n" +
             " Preso in carico da " + CORRIERI[i].nome + " " + CORRIERI[i].cognome + " (ID: " + std::to_string(CORRIERI[i].ID) + ")\n";
            if (ORDINI[i].DataConsegna != NULL) ordine += "Consegnato il " + std::to_string(ORDINI[i].DataConsegna) + "\n";
	        send(clientSocket, ordine.c_str(), ordine.length(), 0);
        }
    } else {
        std::string vuoto = "Non ci sono ordini registrati!\n";
        send(clientSocket, vuoto.c_str(), vuoto.length(), 0); // Invia la frase all'utente
    }
    return;
}