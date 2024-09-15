#include "gestisciCarrello.h"

bool gestisciCarrello(int clientSocket)
{
    int CUSTOMER_ID;
    int RIGHE;
    char buffer[1024] = {0};
    char comando[1000];
    int OPERAZIONI_DISPONIBILI = 5;
    std::pair <int, Prodotto*> risultato;
    std::string OPERAZIONI[] = {"1) Aggiungi prodotto (normale)\n", "2) Aggiungi prodotto (per nome)\n", "3) Rimuovi prodotto dal carrello \n", "4) Ordina prodotti nel carrello \n" , "Altrimenti digita Q per terminare\n"};
    Prodotto* CARRELLO;
    PGresult *res;
    redisContext *c2r; // c2r contiene le info sul contesto
	redisReply *reply; // reply contiene le risposte da Redis

	c2r = redisConnect(REDIS_IP, REDIS_PORT); // Effettua la connessione a Redis
	Con2DB db(HOSTNAME, DB_PORT, USERNAME_CUST, PASSWORD_CUST, DB_NAME); // Effettua la connessione al database

    reply = RedisCommand(c2r, "XREVRANGE %s + - COUNT 1", READ_STREAM);
    if (reply == nullptr || reply->type != REDIS_REPLY_ARRAY || reply->elements == 0)
    {
        std::cerr << "Errore nel comando Redis o stream vuoto" << std::endl;
    }

    std::string id = reply->element[0]->element[1]->element[1]->str; 
    CUSTOMER_ID = stoi(id); // ID Trasportatore
    // Usa una funzione ausiliaria per recuperare il carrello dell'utente
    risultato = recuperaCarrello(clientSocket);
    if (risultato.first == -1) return false;  // C'è stato un errore nella query
    
    RIGHE = risultato.first;
    CARRELLO = risultato.second;
    bool terminaConnessione = false;
    
    while(!terminaConnessione)
    {
        // Mostra all'utente gli elementi nel carrello tramite una funzione ausiliaria
        mostraCarrello(clientSocket, CARRELLO, RIGHE);
        std::string request = "\nQuale operazione vuoi svolgere?\n";
	    send(clientSocket, request.c_str(), request.length(), 0); // Invia il messaggio pre-impostato all'utente
        for (int i = 0; i < OPERAZIONI_DISPONIBILI; i++) send(clientSocket, OPERAZIONI[i].c_str(), OPERAZIONI[i].length(), 0);
        bool attendiInput = true;
        while (attendiInput)
        {
            bool esito = false;
            int bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
            if (bytesRead > 0) 
            {
                std::string messaggio(buffer, bytesRead);
                messaggio.erase(std::remove(messaggio.begin(), messaggio.end(), '\n'), messaggio.end()); // Rimuove eventuali newline
                if (messaggio == "q" || messaggio == "Q") 
                {
                    attendiInput = false;
                    terminaConnessione = true;
                } else if (isdigit(messaggio[0])){
                    int opzione = std::stoi(messaggio) - 1;
                    bool esito = false;
                    switch (opzione)
                    {
                        case 0:
                            // Permette ad un Customer di cercare un prodotto e aggiungerlo al carrello
                            esito = ricercaProdotti(clientSocket);
                            attendiInput = false;
                            break;
                        case 1:
                            // Ancora da implementare la ricerca dei prodotti
                            attendiInput = false;
                            break;
                        case 2:
                            if (RIGHE > 0)
                            {
                                std::string request = "Quale prodotto vuoi rimuovere? (Digita il numero)\n";
	                            send(clientSocket, request.c_str(), request.length(), 0); // Invia il messaggio pre-impostato all'utente
                                int indice = riceviIndice(clientSocket, RIGHE);
                                sprintf(comando, "DELETE FROM prodincart WHERE prodotto = %d AND carrello = %d", CARRELLO[indice].ID, CUSTOMER_ID);
                                try
                                {
                                    res = db.ExecSQLcmd(comando);
                                    PQclear(res);
                                    std::string successo = "Prodotto rimosso correttamente dal carrello!\n";
	                                send(clientSocket, successo.c_str(), successo.length(), 0); // Invia il messaggio pre-impostato all'utente
                                    esito = true;
                                }
                                catch(...)
                                {
                                    std::string errore = "C'è stato un errore nel database!\n";
	                                send(clientSocket, errore.c_str(), errore.length(), 0); // Invia il messaggio pre-impostato all'utente
                                    // Se ci sono errori nella query, vengono catturati da catch
                                }
                            } else {
                                std::string request = "\nNessun prodotto nel carrello da rimuovere!\n";
                                send(clientSocket, request.c_str(), request.length(), 0); // Invia il messaggio pre-impostato all'utente
                            }
                            attendiInput = false;
                            break;
                        case 3:
                            esito = effettuaOrdine(clientSocket, CUSTOMER_ID, RIGHE, CARRELLO);
                            attendiInput = false;
                            break;
                        default:
                            std::string errore = "Opzione non valida, riprova.\n";
                            send(clientSocket, errore.c_str(), errore.length(), 0);
                            break;                               
                    }
                } else {
                    std::string errore = "Input non valido, riprova.\n";
                    send(clientSocket, errore.c_str(), errore.length(), 0);
                }
            } else {
                std::string errore = "Input non valido, riprova.\n";
                send(clientSocket, errore.c_str(), errore.length(), 0);
            }
            if (esito)
            {
                delete[] risultato.second;
                risultato = recuperaCarrello(clientSocket);
                RIGHE = risultato.first;
                CARRELLO = risultato.second;
            }
        }   
    }
    delete[] risultato.second; // Libera la memoria occupata dal carrello
    return true;
}

bool effettuaOrdine(int clientSocket, int customerID, int RIGHE_CARRELLO, Prodotto* CARRELLO)
{
    /*
        Sono richiesti all'utente 2 dati:
        Uno degli indirizzi e uno dei metodi di pagamento tra quelli da lui posseduti
    */
    char buffer[1024] = {0};
    char comando[1000];
	int datiRichiesti = 2;
	int datiRicevuti = 0;
    std::pair<int, Indirizzo*> risultato1;
    std::pair<int, Metodo*> risultato2;
    int RIGHE_INDIRIZZI;
    Indirizzo* INDIRIZZI;
    int RIGHE_METODI;
    Metodo* METODI;
    std::string FRASI[] = {"A quale indirizzo vuoi effettuare la consegna? (Digita il numero)\n", "Con quale metodo di pagamento vuoi pagare? (Digita il numero)\n"};
    PGresult *res;
    Con2DB db(HOSTNAME, DB_PORT, USERNAME_CUST, PASSWORD_CUST, DB_NAME); // Effettua la connessione al database

    // Recupera gli indirizzi registrati dal Customer
    risultato1 = recuperaIndirizzi(clientSocket);
    RIGHE_INDIRIZZI = risultato1.first;
    INDIRIZZI = risultato1.second;
    if (RIGHE_INDIRIZZI == -1) return false;
    else if (RIGHE_INDIRIZZI == 0)
    {
        std::string vuoto = "Non ci sono indirizzi registrati. Operazione interrotta\n\n";
	    send(clientSocket, vuoto.c_str(), vuoto.length(), 0); // Invia il messaggio pre-impostato all'utente
        return false;
    }

    // Recupera i metodi di pagamento registrati dal Customer
    risultato2 = recuperaMetodi(clientSocket);
    RIGHE_METODI = risultato2.first;
    METODI = risultato2.second;
    if (RIGHE_METODI == -1) return false;
    else if (RIGHE_METODI == 0)
    {
        std::string vuoto = "Non ci sono metodi di pagamento registrati. Operazione interrotta\n\n";
	    send(clientSocket, vuoto.c_str(), vuoto.length(), 0); // Invia il messaggio pre-impostato all'utente
        return false;
    }

    int indIndirizzo = -1;
    int indMetodo = -1;
    while(datiRicevuti < datiRichiesti)
    {
		std::string request = FRASI[datiRicevuti]; // Seleziona la frase del turno
		send(clientSocket, request.c_str(), request.length(), 0); // Invia il messaggio pre-impostato all'utente
		int bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0); // Riceve la risposta dall'utente e la memorizza nello stream
		if (bytesRead > 0)
		{
            switch (datiRicevuti)
            {
                case 0:
                    mostraIndirizzi(clientSocket, RIGHE_INDIRIZZI, INDIRIZZI);
                    indIndirizzo = riceviIndice(clientSocket, RIGHE_INDIRIZZI);
                    datiRicevuti++;
                    break;
                case 1:
                    mostraMetodi(clientSocket, RIGHE_METODI, METODI);
                    indMetodo= riceviIndice(clientSocket, RIGHE_METODI);
                    datiRicevuti++;
                    break;
            }
        } else {
            std::string errore = "Input non valido, riprova.\n";
            send(clientSocket, errore.c_str(), errore.length(), 0);
        }
    }
    
    try
    {
        //Inserisce l'ordine nel database
        sprintf(comando, "INSERT INTO ordine(customer, datarich, pagamento, indirizzo) VALUES (%d, NOW, '%s', %d) RETURNING id",
        customerID, METODI[indMetodo].tipo, INDIRIZZI[indIndirizzo].ID);
        res = db.ExecSQLtuples(comando);
        if (PQresultStatus(res) != PGRES_TUPLES_OK) return false; // Controlla che la query sia andata a buon fine
	    int idOrd = atoi(PQgetvalue(res, 0, PQfnumber(res, "id"))); // Recupera l'ID dell'ordine appena creato
        PQclear(res);

        // Ogni prodotto viene inserito nell'ordine
        for (int i = 0; i < RIGHE_CARRELLO; i++)
        {
            sprintf(comando, "INSERT INTO prodinord(prodotto, ordine, quantita) VALUES (%d, %d, %d)",
            CARRELLO[i].ID, idOrd, CARRELLO[i].quantita);
            res = db.ExecSQLcmd(comando);
            PQclear(res);
        }
        std::string successo = "Ordine effettuato correttamente!\n\n";
	    send(clientSocket, successo.c_str(), successo.length(), 0); // Invia il messaggio pre-impostato all'utente
        delete[] risultato1.second;
        delete[] risultato2.second;
        return true;

    }
    catch(...)
    {
        std::string errore = "C'è stato un problema nel database\n\n";
	    send(clientSocket, errore.c_str(), errore.length(), 0); // Invia il messaggio pre-impostato all'utente
        delete[] risultato1.second;
        delete[] risultato2.second;
        return false;
    }
}
