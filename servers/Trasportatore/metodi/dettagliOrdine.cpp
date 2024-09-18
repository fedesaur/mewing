#include "dettagliOrdine.h"

std::tuple<int, Ordine*, Indirizzo*> dettagliOrdine(int clientSocket)  
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

    sprintf(comando, "SELECT ord.id, cst.mail, ord.datarich, ord.stato, ord.pagamento, ind.via, ind.civico, ind.cap, ind.città, ind.stato AS statoIND, ord.totale"
    "FROM indirizzo ind, customers cst, ordine ord, transord tr WHERE ord.id = tr.ordine "
    "AND ind.id = ord.indirizzo AND ord.customer = cst.id AND tr.trasportatore = %d "
    "ORDER BY ord.datarich", COURIER_ID);
    try
    {
        //Recupera tutti gli ordini presi in carico dal trasportatore e non ancora consegnati
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
                const char* mail = PQgetvalue(res, i, PQfnumber(res, "mail"));
                unsigned char* data = (unsigned char*) PQgetvalue(res, i, PQfnumber(res, "datarich"));
                time_t time = static_cast<time_t>(std::stoll(reinterpret_cast<char*>(data))); // Converte il timestamp in time_t
                double statoOrd = atof(PQgetvalue(res, i, PQfnumber(res, "stato")));
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
            std::tuple<int, Ordine*, Indirizzo*> risultato(rows, ordiniDisponibili, indirizzoOrdini);
            return risultato;
        }
        else
        {
            // Non è stato trovato nessun ordine in corso
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

void mostraCorrenti(int clientSocket, int RIGHE, Ordine* ORDINI, Indirizzo* INDIRIZZI)
{
    int rows;
    char comando[1000];
    Prodotto* PRODOTTI_ORDINI;
    PGresult *res;
    Con2DB db(HOSTNAME, DB_PORT, USERNAME_TRAS, PASSWORD_TRAS, DB_NAME); // Effettua la connessione al database
    if (RIGHE > 0)
    {
        std::string request = "\nORDINI PRESI IN CARICO:\n";  // Invia il messaggio pre-impostato all'utente
	    send(clientSocket, request.c_str(), request.length(), 0);
        for (int i = 0; i < RIGHE; i++)
        {
            // Recupera gli attributi degli ordini in corso...
            int ID = ORDINI[i].ID;
            const char* mail = ORDINI[i].MailCustomer;
            time_t data = ORDINI[i].DataRichiesta;
            const char* paga = ORDINI[i].Pagamento;
            double totale = ORDINI[i].Totale;

            //...degli indirizzi in cui effettuare la consegna...
            const char* via = INDIRIZZI[i].via;
            int civico = INDIRIZZI[i].civico;
            const char* CAP = INDIRIZZI[i].CAP;
            const char* city = INDIRIZZI[i].citta;
            const char* stato = INDIRIZZI[i].stato = stato;
            
            // ...e li invia all'utente così che possa visualizzarli ed effettuarci operazioni
            std::string ordine = std::to_string(i+1) + ") ID Ordine: " + std::to_string(ID) +
             " Mail Customer: " + mail + 
             " Data Richiesta: " + std::to_string(data) + 
             " Metodo Pagamento: " + paga +
             " Totale Ordine: " + std::to_string(totale) + "\n" + 
             " Da consegnare in: " + via + " " + std::to_string(civico) + ", " + city + " (CAP : " + CAP + "), " + stato + "\n";
	        send(clientSocket, ordine.c_str(), ordine.length(), 0);
            
            // Vengono recuperate le informazioni dei prodotti presenti negli ordini
            sprintf(comando, "SELECT pr.id, pr.descrizione, pr.nome, pr.prezzo, fr.nome AS nomeF, pn.quantita "
            "FROM prodotto pr, prodinord pn, fornitore fr WHERE pn.prodotto = pr.id "
            "AND pr.fornitore = fr.id AND pn.ordine = %d", ID);
            try
            {
                res = db.ExecSQLtuples(comando);
                rows = PQntuples(res); // Si presume ci siano prodotti nell'ordine
                std::string premessa = "PRODOTTI PRESENTI NELL'ORDINE:\n";
	            send(clientSocket, premessa.c_str(), premessa.length(), 0);
                for (int j = 0; j < rows; j++)
                {
                    int IDProd = atoi(PQgetvalue(res, j, PQfnumber(res, "id")));
                    const char* descrizione = PQgetvalue(res, j, PQfnumber(res, "descrizione"));
                    double prezzo = atof(PQgetvalue(res, j, PQfnumber(res, "prezzo")));
                    const char* nome = PQgetvalue(res, j, PQfnumber(res, "nome"));
                    const char* fornitore = PQgetvalue(res, j, PQfnumber(res, "nomeF"));
                    int quantita = atoi(PQgetvalue(res, j, PQfnumber(res, "quantita")));

                    std::string prodotto = "\t" + std::to_string(j+1) + ") ID Prodotto: " + std::to_string(IDProd) +
                    " Nome Prodotto: " + nome + 
                    " Descrizione: " + descrizione + 
                    " Fornitore: " + fornitore + 
                    " Prezzo Prodotto: " + std::to_string(prezzo) + 
                    " Quantità :" + std::to_string(quantita) + "\n";
	                send(clientSocket, prodotto.c_str(), prodotto.length(), 0);
                }
                PQclear(res); // Libera lo spazio occupato dai risultati della query
            }
            catch(...)
            {
                std::string errore = "C'è stato un problema con il database\n";
                send(clientSocket, errore.c_str(), errore.length(), 0);
                return;
            }
        }
    } else {
        std::string vuoto = "Non ci sono ordini presi in carico!\n";
        send(clientSocket, vuoto.c_str(), vuoto.length(), 0); // Invia la frase all'utente
    }
    return;
}