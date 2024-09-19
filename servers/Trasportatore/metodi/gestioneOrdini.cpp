#include "gestioneOrdini.h"

bool gestioneOrdini(int clientSocket)
{
    int TRASPORTER_ID;
    int RIGHE;
    char buffer[1024] = {0};
    int OPERAZIONI_DISPONIBILI = 4;
    std::string OPERAZIONI[] = {"1) Recupera dettagli ordine\n", "2) Ricerca nuovo ordine\n", "3) Conferma consegna di un ordine\n", "Altrimenti digita Q per terminare\n"};
    std::tuple<int, Ordine*, Corriere*> risultato;
    Corriere* CORRIERI;
    Ordine* ORDINI;
    redisContext *c2r; // c2r contiene le info sul contesto
	redisReply *reply; // reply contiene le risposte da Redis

	c2r = redisConnect(REDIS_IP, REDIS_PORT); // Effettua la connessione a Redis
    reply = RedisCommand(c2r, "XREVRANGE %s + - COUNT 1", READ_STREAM);
    if (reply == nullptr || reply->type != REDIS_REPLY_ARRAY || reply->elements == 0)
    {
        std::cerr << "Errore nel comando Redis o stream vuoto" << std::endl;
    }

    std::string id = reply->element[0]->element[1]->element[1]->str; 
    TRASPORTER_ID = stoi(id); // ID Trasportatore
    // Usa una funzione ausiliaria per recuperare gli ordini in corso di consegna
    risultato = ordiniCorrenti(clientSocket);
    if (std::get<0>(risultato) == -1) return false;  // C'è stato un errore nella query
    
    bool terminaConnessione = false;
    while(!terminaConnessione)
    {
        RIGHE = std::get<0>(risultato);
        ORDINI = std::get<1>(risultato);
        CORRIERI = std::get<2>(risultato);
        
        // Mostra i corrieri e i loro ordini
        mostraCorrenti(clientSocket,RIGHE,CORRIERI,ORDINI);
        std::string request = "\nQuale operazione vuoi svolgere?\n";
	    send(clientSocket, request.c_str(), request.length(), 0); // Invia il messaggio pre-impostato all'utente
        
        // Mostra le operazioni disponibili
        for (int i = 0; i < OPERAZIONI_DISPONIBILI; i++) send(clientSocket, OPERAZIONI[i].c_str(), OPERAZIONI[i].length(), 0);
        std::tuple<int, Ordine*, Indirizzo*> temp;
        bool attendiInput = true;
        while (attendiInput)
        {
            int bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
            if (bytesRead > 0) 
            {
                std::string messaggio(buffer, bytesRead);
                messaggio.erase(std::remove(messaggio.begin(), messaggio.end(), '\n'), messaggio.end()); // Rimuove eventuali newline

                if (messaggio == "q" || messaggio == "Q") 
                {
                    attendiInput = false;
                    terminaConnessione = true;
                } else if (isdigit(messaggio[0])) { //isNumber è una funzione ausiliaria in lib
                    int opzione = std::stoi(messaggio) - 1;
                    bool esito = false;
                    switch (opzione)
                    {
                        case 0:
                            if (RIGHE > 0)
                            {
                                std::string request = "Di quale ordine vuoi recuperare i dettagli?(Digita il numero)\n";
	                            send(clientSocket, request.c_str(), request.length(), 0); 
                                int indice = riceviIndice(clientSocket, RIGHE);
                                dettagliOrdine(clientSocket, ORDINI[indice].ID);
                            } else {
                                std::string errore = "Non ci sono ordini per cui recuperare i dettagli!\n";
                                send(clientSocket, errore.c_str(), errore.length(), 0);
                            }
                            attendiInput = false;
                            break;
                        case 1:
                            
                            esito = prendiOrdine(clientSocket, TRASPORTER_ID);
                            attendiInput = false;
                            break;
                        case 2:
                            if (RIGHE > 0)
                            {
                                std::string request = "Di quale ordine vuoi confermare la consegna?(Digita il numero)\n";
	                            send(clientSocket, request.c_str(), request.length(), 0); 
                                int indice = riceviIndice(clientSocket, RIGHE);
                                esito = consegnaOrdine(clientSocket, ORDINI[indice].ID);
                            } else {
                                std::string errore = "Non ci sono ordini per cui confermare la consegna!\n";
                                send(clientSocket, errore.c_str(), errore.length(), 0);
                            }
                            attendiInput = false;
                            break;
                        default:
                            std::string errore = "Opzione non valida, riprova.\n";
                            send(clientSocket, errore.c_str(), errore.length(), 0);
                            break;                               
                    }
                    if (esito)
                    {
                        delete[] ORDINI;
                        delete[] CORRIERI;
                        risultato = ordiniCorrenti(clientSocket);
                    }
                } else {
                    std::string errore = "Input non valido, riprova.\n";
                    send(clientSocket, errore.c_str(), errore.length(), 0);
                }
            } else {
                std::string errore = "Input non valido, riprova.\n";
                send(clientSocket, errore.c_str(), errore.length(), 0);
            }
        }   
    }
    // Libera la memoria occupata da Ordini e Corrieri
    delete[] ORDINI;
    delete[] CORRIERI;
    return true;
}

bool consegnaOrdine(int clientSocket, int ordineID)
{
    char comando[1000];
    PGresult *res;
    Con2DB db(HOSTNAME, DB_PORT, USERNAME_TRAS, PASSWORD_TRAS, DB_NAME); // Effettua la connessione al database
    try
    {
        sprintf(comando, "INSERT INTO ordineconse(id, datacons) VALUES (%d, NOW())", ordineID);
        res = db.ExecSQLcmd(comando);
        std::string request = "Consegna effettuata!";
	    send(clientSocket, request.c_str(), request.length(), 0);
        PQclear(res);
        return true; 
    }
    catch(...)
    {
        std::string errore = "C'è stato un errore nel database\n";
		send(clientSocket, errore.c_str(), errore.length(), 0); // Invia il messaggio pre-impostato all'utente
        PQclear(res);
        return false;
    }
    
}

void dettagliOrdine(int clientSocket, int ordineID)  
{
    int rows;
    char comando[1000];
    PGresult *res;
	Con2DB db(HOSTNAME, DB_PORT, USERNAME_TRAS, PASSWORD_TRAS, DB_NAME); // Effettua la connessione al database

    try
    {
        //Recupera tutti gli ordini presi in carico dal trasportatore e non ancora consegnati
        sprintf(comando, "SELECT cst.mail, ord.datarich, ord.stato, ord.pagamento, ind.via, ind.civico, ind.cap, ind.città, ind.stato AS statoIND, ord.totale"
        "FROM indirizzo ind, customers cst, ordine ord "
        "WHERE ind.id = ord.indirizzo AND ord.customer = cst.id AND ord.id = %d", ordineID);
        res = db.ExecSQLtuples(comando);
        if (PQntuples(res) == 0) return;
        // Recupera gli attributi dell'ordine...
        const char* mail = PQgetvalue(res, 0, PQfnumber(res, "mail"));
        unsigned char* data = (unsigned char*) PQgetvalue(res, 0, PQfnumber(res, "datarich"));
        time_t time = static_cast<time_t>(std::stoll(reinterpret_cast<char*>(data))); // Converte il timestamp in time_t
        const char* statoOrd = PQgetvalue(res, 0, PQfnumber(res, "stato"));
        const char* paga = PQgetvalue(res, 0, PQfnumber(res, "pagamento"));
        
        //...e dell'indirizzo di consegna
        const char* via = PQgetvalue(res, 0, PQfnumber(res, "via"));
        int civico = atoi(PQgetvalue(res, 0, PQfnumber(res, "civico")));
        const char* CAP = PQgetvalue(res, 0, PQfnumber(res, "cap"));
        const char* city = PQgetvalue(res, 0, PQfnumber(res, "citta"));
        const char* stato = PQgetvalue(res, 0, PQfnumber(res, "statoIND"));
        double totale = atof(PQgetvalue(res, 0, PQfnumber(res, "totale")));

        std::string ordine = "ID Ordine: " + std::to_string(ordineID) +
             " Mail Customer: " + mail + 
             " Data Richiesta: " + std::to_string(time) + 
             " Metodo Pagamento: " + paga +
             " Totale Ordine: " + std::to_string(totale) + "\n" + 
             " Da consegnare in: " + via + " " + std::to_string(civico) + ", " + city + " (CAP : " + CAP + "), " + stato + "\n";
	    send(clientSocket, ordine.c_str(), ordine.length(), 0);
        PQclear(res);
        //Poi recupera le informazioni dei prodotti al suo interno

        sprintf(comando, "SELECT pr.id, pr.descrizione, pr.nome, pr.prezzo, fr.nome AS nomeF, pn.quantita "
            "FROM prodotto pr, prodinord pn, fornitore fr WHERE pn.prodotto = pr.id "
            "AND pr.fornitore = fr.id AND pn.ordine = %d", ordineID);
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
    }
    catch(...)
    {
        std::string errore = "C'è stato un problema con il database\n";
        send(clientSocket, errore.c_str(), errore.length(), 0);
    }
    PQclear(res); // Libera lo spazio occupato dai risultati della query
    return;
}

bool prendiOrdine(int clientSocket, int trasporterID)
{
    int rows;
    char comando[1000];
    int RIGHE_CORRIERI;
    int RIGHE_ORDINI;
    Ordine* ORDINI;
    Indirizzo* INDIRIZZI;
    Corriere* CORRIERI;
    PGresult *res;
	Con2DB db(HOSTNAME, DB_PORT, USERNAME_TRAS, PASSWORD_TRAS, DB_NAME); // Effettua la connessione al database

    std::pair<int, Corriere*> risultato1 = recuperaCorrieri(clientSocket);
    if (risultato1.first == -1) return false;
    else if (risultato1.first == 0)
    {
        std::string errore = "Non ci sono corrieri disponibili per effettuare gli ordini\n";
        send(clientSocket, errore.c_str(), errore.length(), 0);
        return false;
    }
    RIGHE_CORRIERI = risultato1.first;
    CORRIERI = risultato1.second;

    std::tuple<int, Ordine*, Indirizzo*> risultato2 = ricercaOrdini(clientSocket);  
    if (std::get<0>(risultato2) == -1) return false;
    else if (std::get<0>(risultato2) == 0)
    {
        std::string vuoto = "Non ci sono ordini disponibili da prendere in carico!\n";
        send(clientSocket, vuoto.c_str(), vuoto.length(), 0);
        delete[] CORRIERI;
        return false;
    }

    int RIGHE_ORDINI = std::get<0>(risultato2);
    Ordine* ORDINI = std::get<1>(risultato2);
    Indirizzo* INDIRIZZI = std::get<2>(risultato2);
    // Mostra all'utente gli ordini disponibili per le consegne
    mostraOrdini(clientSocket, RIGHE_ORDINI, ORDINI, INDIRIZZI);
    
    std::string request = "Quale ordine vuoi prendere in carico?\n";
	send(clientSocket, request.c_str(), request.length(), 0); 
    int indiceOrd = riceviIndice(clientSocket, RIGHE_ORDINI);
    int idOrd = ORDINI[indiceOrd].ID;

    mostraCorrieri(clientSocket, RIGHE_CORRIERI, CORRIERI);
    int indiceCor = riceviIndice(clientSocket, RIGHE_CORRIERI);
    int idCor = CORRIERI[indiceCor].ID

    try
    {
        // Prende in carico l'ordine
        sprintf(comando, "INSERT INTO transord(ordine, trasportatore) VALUES(%d, %d)", idOrd, trasporterID);
        res = db.ExecSQLcmd(comando);
        PQclear(res);
        // Assegna l'ordine ad un corriere
        sprintf(comando, "INSERT INTO consegna(ordine, corriere) VALUES (%d, %d)", idOrd, idCor);
        res = db.ExecSQLcmd(comando);
        PQclear(res);
        // Aggiorna l'ordine indicando che è stato preso in carico
        sprintf(comando, "UPDATE ordine SET stato = 'accettato' WHERE id = %d", idOrd);
        res = db.ExecSQLcmd(comando);
        PQclear(res);
        std::string successo = "L'ordine è stato correttamente preso in carico!\n";
        send(clientSocket, successo.c_str(), successo.length(), 0);
        delete[] CORRIERI; // Libera lo spazio occupato dai vari array
        delete[] ORDINI;
        delete[] INDIRIZZI;
        return true;
    }
    catch(...)
    {
        std::string errore = "C'è stato un problema con il database\n";
        send(clientSocket, errore.c_str(), errore.length(), 0);
        delete[] CORRIERI; // Libera lo spazio occupato dai vari array
        delete[] ORDINI;
        delete[] INDIRIZZI;
        return false;
    }
}