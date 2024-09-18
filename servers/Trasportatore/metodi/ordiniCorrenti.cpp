#include "ordiniCorrenti.h"

std::tuple<int, Ordine*, Indirizzo*> ordiniCorrenti(int clientSocket)  
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

    // Seleziona tutti i corrieri e gli ordini da loro presi in carico
    sprintf(comando, "SELECT ord.id, cst.mail, ord.datarich, ord.stato, ord.pagamento, ord.totale, cor.id AS CorID, cor.nome, cor.cognome"
    "FROM customers cst, ordine ord, consegna cons, corriere cor, WHERE ord.id = cons.ordine "
    "AND cor.id = cons.corriere AND ord.customer = cst.id AND cor.azienda = %d AND ord.id NOT IN (SELECT id FROM ordineconse) "
    "ORDER BY cor.id", COURIER_ID);
    try
    {
        //Recupera tutti gli ordini presi in carico dal trasportatore e non ancora consegnati
        res = db.ExecSQLtuples(comando);
        rows = PQntuples(res);
        if (rows > 0)
        {
            Ordine* ORDINI = new Ordine[rows];
            Corriere* CORRIERI = new Corriere[rows];
            for (int i = 0; i < rows; i++)
            {
                // Recupera gli attributi degli ordini e degli indirizzi della query sopra svolta...
                int ID = atoi(PQgetvalue(res, i, PQfnumber(res, "id")));
                const char* mail = PQgetvalue(res, i, PQfnumber(res, "mail"));
                unsigned char* data = (unsigned char*) PQgetvalue(res, i, PQfnumber(res, "datarich"));
                time_t time = static_cast<time_t>(std::stoll(reinterpret_cast<char*>(data))); // Converte il timestamp in time_t
                const char* statoOrd = atof(PQgetvalue(res, i, PQfnumber(res, "stato")));
                const char* paga = PQgetvalue(res, i, PQfnumber(res, "pagamento"));
                double totale = atof(PQgetvalue(res, i, PQfnumber(res, "totale")));
                int IDCor = atoi(PQgetvalue(res, i, PQfnumber(res, "CorID")));
                const char* nome = PQgetvalue(res, i, PQfnumber(res, "nome"));
                const char* cognome = PQgetvalue(res, i, PQfnumber(res, "cognome"));

                //...e li assegna all'i-esimo Ordine...
                ORDINI[i].ID = ID;
                ORDINI[i].MailCustomer = mail;
                ORDINI[i].DataRichiesta = time;
                ORDINI[i].Stato = statoOrd;
                ORDINI[i].Pagamento = paga;
                ORDINI[i].Totale = totale;

                //...e all'i-esimo Corriere (ci saranno duplicati)
                CORRIERI[i].ID = IDCor;
                CORRIERI[i].nome = nome;
                CORRIERI[i].cognome = cognome;
            }
            std::tuple<int, Ordine*, Corriere*> risultato(rows, ORDINI, CORRIERI);
            return risultato;
        }
        else
        {
            // Non è stato trovato nessun ordine in corso
            std::tuple<int, Ordine*, Corriere*> vuoto(0,nullptr, nullptr);
            return vuoto;
        }
    }
    catch(...)
    {
        std::string errore = "C'è stato un problema con il database\n";
        send(clientSocket, errore.c_str(), errore.length(), 0);
        std::tuple<int, Ordine*, Corriere*> ritorno(-1, nullptr, nullptr);
        return ritorno;
    }
}

void mostraCorrenti(int clientSocket, int RIGHE, Corriere* CORRIERI, Ordine* ORDINI)
{
    if (RIGHE > 0)
    {
        std::string request = "\nORDINI PRESI IN CARICO:\n";  // Invia il messaggio pre-impostato all'utente
	    send(clientSocket, request.c_str(), request.length(), 0);
        int index = 0;
        int corrNow = 0;
        while (index < RIGHE)
        {
            // Recupera le informazioni dei vari corrieri...
            int IDCor = CORRIERI[index].ID;
            const char* nome = CORRIERI[index].nome;
            const char* cognome = CORRIERI[index].cognome;
            // ... e le stampa
            std::string corr = std::to_string(corrNow+1) + ") ID Corriere: " + std::to_string(IDCor) +
             " Nome Corriere: " + nome + 
             " Cognome Corriere: " + cognome + "\n" + "Ha in carico:\n";
	        send(clientSocket, corr.c_str(), corr.length(), 0);
            
            /*
                Di ogni corriere, recupera le informazioni sui suoi ordini
                e le stampa. NOTA: La query sopra riportata ha dei duplicati
                tra i Corrieri, perciò si scorreranno solo gli ordini finché non
                si arriva ad un nuovo Corriere (si vede in base all'ID)
            */
            while (IDCor == CORRIERI[index].ID && index < RIGHE)
            {
                int ID = ORDINI[index].ID;
                const char* mail = ORDINI[index].MailCustomer;
                time_t data = ORDINI[index].DataRichiesta;
                const char* paga = ORDINI[index].Pagamento;
                double totale = ORDINI[index].Totale;
                
                std::string ordine = "\t" + std::to_string(index+1) + ") ID Ordine: " + std::to_string(ID) +
                " Mail Customer: " + mail + 
                " Data Richiesta: " + std::to_string(data) + 
                " Metodo Pagamento: " + paga +
                " Totale Ordine: " + std::to_string(totale) + "\n";
                send(clientSocket, ordine.c_str(), ordine.length(), 0);
                index++;
            }
            corrNow++;
        }
    } else {
        std::string vuoto = "Non ci sono ordini presi in carico!\n";
        send(clientSocket, vuoto.c_str(), vuoto.length(), 0); // Invia la frase all'utente
    }
    return;
}