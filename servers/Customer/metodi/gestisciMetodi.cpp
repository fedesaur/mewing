#include "gestisciMetodi.h"

bool gestisciIndirizzi(int clientSocket)
{
    int CUSTOMER_ID;
    int RIGHE;
    char buffer[1024] = {0};
    char comando[1000];
    int OPERAZIONI_DISPONIBILI = 3;
    std::pair <int, Metodo*> risultato;
    std::string OPERAZIONI[] = {"1) Aggiungi metodo di pagamento\n", "2) Rimuovi metodo di pagamento\n", "Altrimenti digita Q per terminare\n"};
    Metodo* METODI;
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
    CUSTOMER_ID = std::stoi(id); // ID Customer
    
    // Usa una funzione ausiliaria per recuperare gli indirizzi registrati
    risultato = recuperaIndirizzi(clientSocket);
    if (risultato.first == -1)
    {
        std::string errore = "C'è stato un errore nel database\n\n";
	    send(clientSocket, errore.c_str(), errore.length(), 0); // Invia il messaggio pre-impostato all'utente
         return false;  // C'è stato un errore nella query
    }
    
    RIGHE = risultato.first;
    METODI = risultato.second;
    bool terminaConnessione = false;
    
    while(!terminaConnessione)
    {
        // Mostra all'utente gli elementi nel carrello tramite una funzione ausiliaria
        mostraMetodi(clientSocket, RIGHE, METODI);
        std::string request = "Quale operazione vuoi svolgere?\n";
	    send(clientSocket, request.c_str(), request.length(), 0); // Invia il messaggio pre-impostato all'utente
        for (int i = 0; i < OPERAZIONI_DISPONIBILI; i++) send(clientSocket, OPERAZIONI[i].c_str(), OPERAZIONI[i].length(), 0);
        bool attendiInput = true;
        bool esito = false;
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
                } else if (isdigit(messaggio[0])){
                    int opzione = std::stoi(messaggio) - 1;
                    bool esito = false;
                    switch (opzione)
                    {
                        case 0:
                        {
                            // Permette ad un Customer di aggiungere un Indirizzo
                            esito = aggiungiMetodo(clientSocket);
                            attendiInput = false;
                            break;
                        }
                        case 1:
                        {
                            std::string request = "Quale metodo vuoi rimuovere? Digita il numero\n";
	                        send(clientSocket, request.c_str(), request.length(), 0); // Invia il messaggio pre-impostato all'utente
                            int index = riceviIndice(clientSocket, RIGHE);
                            try
                            {
                                int idInd = METODI[index].ID;
                                sprintf(comando, "DELETE FROM metpag WHERE id = %d AND Customer = %d", idInd, CUSTOMER_ID);
                                res = db.ExecSQLcmd(comando);
                                std::string request = "Metodo rimosso con successo!\n\n";
	                            send(clientSocket, request.c_str(), request.length(), 0); // Invia il messaggio pre-impostato all'utente
                                esito = true;
                                attendiInput = false;
                            }
                            catch(...)
                            {
                                std::string errore = "C'è stato un errore nel database\n\n";
		                        send(clientSocket, errore.c_str(), errore.length(), 0); // Invia il messaggio pre-impostato all'utente   
                            }
                            break;
                        }
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
        }
        if (esito)
        {
            delete[] risultato.second;
            risultato = recuperaMetodi(clientSocket);
            RIGHE = risultato.first;
            METODI = risultato.second;
        }
           
    }
    delete[] risultato.second; // Libera la memoria occupata dal carrello
    return true;
}

std::pair <int, Metodo*> recuperaMetodi(int CUSTOMER_ID)
{
    std::pair <int, Metodo*> risultato;
    int RIGHE;
    char comando[1000];
    PGresult *res;
	Con2DB db(HOSTNAME, DB_PORT, USERNAME_CUST, PASSWORD_CUST, DB_NAME); // Effettua la connessione al database
    
    sprintf(comando, "SELECT * FROM metpag WHERE Customer = %d", CUSTOMER_ID);
    try
    {
        res = db.ExecSQLtuples(comando);
        RIGHE = PQntuples(res);
        if (RIGHE > 0)
        {
            
            Metodo* metodi = new Metodo[RIGHE];
            // Recupera gli indirizzi e li memorizza
            for (int i = 0; i < RIGHE; i++)
            {
                // Recupera gli attributi degli dalla query sopra svolta...
                int ID = atoi(PQgetvalue(res, i, PQfnumber(res, "id")));
                const char* nome = PQgetvalue(res, i, PQfnumber(res, "nome"));
                const char* tipo = PQgetvalue(res, i, PQfnumber(res, "tipo"));

                // e li assegna all'i-esimo Indirizzo in indirizzi
                metodi[i].ID = ID;
                metodi[i].nome = nome;
                metodi[i].tipo = tipo;
                metodi[i].Customer = CUSTOMER_ID;
            }
            risultato.first = RIGHE; // Ritorna il numero di righe degli indirizzi
            risultato.second = metodi; // Ritorna l'array degli indirizzi
        } else {     // Se non ci sono indirizzi
        risultato.first = 0;
        risultato.second = nullptr;
        }
        PQclear(res);
        return risultato;
    }
    catch(...)
    {
        risultato.first = -1;
        risultato.second = nullptr;
        PQclear(res);
        return risultato;
    }
}

void mostraMetodi(int clientSocket, int righe, Metodo* metodi)
{
    if (righe > 0)
    {
        std::string request = "\nMETODI REGISTRATI:\n";
	    send(clientSocket, request.c_str(), request.length(), 0); // Invia il messaggio pre-impostato all'utente
        for (int i = 0; i < righe; i++)
        {
            std::string metodo = std::to_string(i+1) + ") ID Metodo: " + std::to_string(metodi[i].ID) +
             " Nome: " + metodi[i].nome +
             " Metodo: " + metodi[i].tipo + "\n";
	        send(clientSocket, metodo.c_str(), metodo.length(), 0);
        }
    } else {
        std::string request = "\nNon ci sono metodi registrati!\n\n"; //... e lo stampa
	    send(clientSocket, request.c_str(), request.length(), 0); // Invia il messaggio pre-impostato all'utente
    }
    return;
}

bool aggiungiMetodo(int clientSocket)
{
	// Sono richiesti 2 dati all'utente: Nome e Tipo
    int CUSTOMER_ID;
	int datiRichiesti = 2;
	int datiRicevuti = 0;
	char comando[1000];
    PGresult *res;
    redisContext *c2r; // c2r contiene le info sul contesto
	redisReply *reply; // reply contiene le risposte da Redischar buffer[1024] = {0};
	c2r = redisConnect(REDIS_IP, REDIS_PORT); // Effettua la connessione a Redis
	Con2DB db(HOSTNAME, DB_PORT, USERNAME_CUST, PASSWORD_CUST, DB_NAME); // Effettua la connessione al database
    
    reply = RedisCommand(c2r, "XREVRANGE %s + - COUNT 1", READ_STREAM);
    if (reply == nullptr || reply->type != REDIS_REPLY_ARRAY || reply->elements == 0)
    {
        std::cerr << "Errore nel comando Redis o stream vuoto" << std::endl;
    }
    std::string id = reply->element[0]->element[1]->element[1]->str; 
    CUSTOMER_ID = stoi(id); // ID Customer

	std::string nome;
    int indice = -1;
	
	// Di seguito, le frasi mostrate all'utente ad ogni fase della creazione del Metodo
    std::string METODI[] = {"Virtuale", "contante", "carta prepagata", "carta di credito", "bancomat"};
    std::string FRASI[] = {"Inserisci il nome del metodo di pagamento\n",
    "Inserisci il tipo del metodo (digita il numero): 1) Virtuale 2) Contante 3) Carta Prepagata 4) Carta di Credito 5) Bancomat\n"};

	// Chiede i dati neccessari per creare il customer e l'indirizzo
	while (datiRicevuti < datiRichiesti)
	{
		std::string temp;
		char buffer[1024] = {0};
		std::string request = FRASI[datiRicevuti]; // Seleziona la frase del turno
		send(clientSocket, request.c_str(), request.length(), 0); // Invia il messaggio pre-impostato all'utente
		int bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0); // Riceve la risposta dall'utente e la memorizza nello stream
		if (bytesRead > 0)
		{
			switch(datiRicevuti)
			{
				case 0:
                    
					temp = buffer;
					temp.pop_back();
                    if (temp.length() > 50 || temp.length() == 0)
                    {
                       	std::string errore = "Il nome del metodo deve essere di massimo 50 caratteri\n"; // Seleziona la frase del turno
						send(clientSocket, errore.c_str(), errore.length(), 0); // Invia il messaggio pre-impostato all'utente 
                    } else {
                        nome = temp;
                        datiRicevuti++;
                    }
					break;
				case 1:
                    indice = riceviIndice(clientSocket, 5);
					break;
			}
        } else {
            std::string errore = "Input non valido, riprova.\n";
            send(clientSocket, errore.c_str(), errore.length(), 0);
        }
	}
	// Viene inserito il nuovo metodo nel database
    try
    {
	    sprintf(comando, "INSERT INTO metpag(nome, tipo) VALUES('%s','%s')", nome.c_str(), tipo.c_str());
        res = db.ExecSQLcmd(comando);
        std::string successo = "Metodo aggiunto al database\n\n";
		send(clientSocket, successo.c_str(), successo.length(), 0); // Invia il messaggio pre-impostato all'utente    
        PQclear(res);
		return true;
    }
    catch(...) //Se c'è stato un errore, lo segnala all'utente
    {
        std::string errore = "C'è stato un errore nel database\n\n";
		send(clientSocket, errore.c_str(), errore.length(), 0); // Invia il messaggio pre-impostato all'utente    
        return false;
    }
    

}