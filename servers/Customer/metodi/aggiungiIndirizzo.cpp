#include "aggiungiIndirizzo.h"

bool aggiungiIndirizzo(int clientSocket)
{
	/* 
		Sono richiesti 5 dati all'utente:
		Via, Civico, CAP, Città, Stato
	*/
    int CUSTOMER_ID;
	int datiRichiesti = 5;
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

	std::string via;
	int civico;
	std::string CAP;
	std::string city;
	std::string stato;
	
	// Di seguito, le frasi mostrate all'utente ad ogni fase della creazione del Customer
	std::string FRASI[] = {"Inserisci la Via del tuo indirizzo\n",
	"Inserisci il Civico del tuo indirizzo\n",
	"Inserisci il CAP del tuo indirizzo\n",
	"Inserisci la Città del tuo indirizzo\n",
	"Inserisci lo Stato del tuo indirizzo\n"};

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
                    if (temp.length() > 100 || temp.length() == 0)
                    {
                       	std::string errore = "La via deve avere al massimo 100 caratteri\n"; // Seleziona la frase del turno
						send(clientSocket, errore.c_str(), errore.length(), 0); // Invia il messaggio pre-impostato all'utente 
                    } else {
                        via = temp;
                        datiRicevuti++;
                    }
					break;
				case 1:
					temp = buffer;
					temp.pop_back();
					if (isNumber(temp) && stoi(temp) >= 0)
					{
						civico = stoi(temp);
						datiRicevuti++;
					} else {
						std::string errore = "Il civico deve essere un numero positivo\n"; // Seleziona la frase del turno
						send(clientSocket, errore.c_str(), errore.length(), 0); // Invia il messaggio pre-impostato all'utente
					}
					break;
				case 2:
					temp = buffer;
					temp.pop_back();
					if (isNumber(temp) && temp.length() == 5)
					{
						CAP = temp;
						datiRicevuti++;
					} else {
						std::string errore = "Il CAP deve essere una sequenza di 5 cifre\n"; // Seleziona la frase del turno
						send(clientSocket, errore.c_str(), errore.length(), 0); // Invia il messaggio pre-impostato all'utente
					}
					break;
				case 3:
					temp = buffer;
					temp.pop_back();
                    if (temp.length() > 30 || temp.length() == 0)
                    {
                       	std::string errore = "La città deve avere al massimo 30 caratteri\n"; // Seleziona la frase del turno
						send(clientSocket, errore.c_str(), errore.length(), 0); // Invia il messaggio pre-impostato all'utente 
                    } else {
                        city = temp;
                        datiRicevuti++;
                    }
					break;
				case 4:
					temp = buffer;
					temp.pop_back();
                    if (temp.length() > 30 || temp.length() == 0)
                    {
                       	std::string errore = "Lo stato deve avere al massimo 30 caratteri\n"; // Seleziona la frase del turno
						send(clientSocket, errore.c_str(), errore.length(), 0); // Invia il messaggio pre-impostato all'utente 
                    } else {
                        stato = temp;
                        datiRicevuti++;
                    }
					break;
			}
        } else {
            std::string errore = "Input non valido, riprova.\n";
            send(clientSocket, errore.c_str(), errore.length(), 0);
        }
	}
	// Viene inserito il nuovo indirizzo nel database
    try
    {
	    sprintf(comando, "INSERT INTO Indirizzo(via, civico, cap, citta, stato) VALUES('%s', %d, '%s', '%s', '%s') RETURNING id", via.c_str(), civico, CAP.c_str(), city.c_str(), stato.c_str());
	    res = db.ExecSQLtuples(comando);
        int address = atoi(PQgetvalue(res, 0, PQfnumber(res, "id"))); // Recupera l'ID dell'indirizzo appena aggiunto
        sprintf(comando, "INSERT INTO custadd(customer, addr) VALUES (%d, %d)", CUSTOMER_ID, address);
        res = db.ExecSQLcmd(comando);
        std::string errore = "Indirizzo aggiunto al database\n\n";
		send(clientSocket, errore.c_str(), errore.length(), 0); // Invia il messaggio pre-impostato all'utente    
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