#include "registraCorriere.h"

bool registraCorriere(int clientSocket)
{
    int COURIER_ID;
    char comando[1000];
    PGresult *res;
    redisContext *c2r; // c2r contiene le info sul contesto
	redisReply *reply; // reply contiene le risposte da Redis

	c2r = redisConnect(REDIS_IP, REDIS_PORT); // Effettua la connessione a Redis
	Con2DB db(HOSTNAME, DB_PORT, USERNAMEC, PASSWORDC, DB_NAME); // Effettua la connessione al database

    reply = RedisCommand(c2r, "XREVRANGE %s + - COUNT 1", READ_STREAM);
    if (reply == nullptr || reply->type != REDIS_REPLY_ARRAY || reply->elements == 0)
    {
        std::cerr << "Errore nel comando Redis o stream vuoto" << std::endl;
    }
    std::string id = reply->element[0]->element[1]->element[1]->str; 
    COURIER_ID = atoi(id.c_str()); // ID Corriere
    std::string nome;
	std::string cognome;
    int datiRichiesti = 2;
	int datiRicevuti = 0;
	
	// Di seguito, le frasi mostrate all'utente ad ogni fase della creazione del Customer
	std::string FRASI[] = {"Inserisci Nome del Corriere\n","Inserisci il tuo Cognome del Corriere\n"};

	// Chiede i dati neccessari per creare il Corriere
	while (datiRicevuti < datiRichiesti)
	{
		char buffer[1024] = {0};
		std::string request = FRASI[datiRicevuti]; // Seleziona la frase del turno
		send(clientSocket, request.c_str(), request.length(), 0); // Invia il messaggio pre-impostato all'utente
		int bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0); // Riceve la risposta dall'utente e la memorizza nello stream
		if (bytesRead > 0)
		{
			// bool correctInput = true; Nel caso i dati non vadano bene, si potrebbe pensare a ripetere quel passaggio
			std::string temp;
			switch(datiRicevuti)
			{
				case 0:
					temp = buffer;
                    temp.pop_back();
                    if (temp.length() > 20 || temp.length() == 0)
                    {
                        std::string errore = "Il Nome del corriere deve essere di massimo 20 caratteri\n";
						send(clientSocket, errore.c_str(), errore.length(), 0); // Invia il messaggio pre-impostato all'utente
                    } else {
                        nome = temp;
                        datiRicevuti++;
                    }
					nome.pop_back();
					break;
				case 1:
                    temp = buffer;
                    temp.pop_back();
                    if (temp.length() > 20 || temp.length() == 0)
                    {
                        std::string errore = "Il Cognome del corriere deve essere di massimo 20 caratteri\n";
						send(clientSocket, errore.c_str(), errore.length(), 0); // Invia il messaggio pre-impostato all'utente
                    } else {
                        cognome = temp;
                        datiRicevuti++;
                    }
					break;
            }
		}
		else
        {
            std::string errore = "C'è stato un errore nella lettura dei dati\n";
			send(clientSocket, errore.c_str(), errore.length(), 0); // Invia il messaggio pre-impostato all'utente
        }
	}
    // Ricevuti i dati, aggiunge il Corriere al database
    sprintf(comando, "INSERT INTO corriere(azienda, nome, cognome) VALUES (%d, '%s', '%s')",
    COURIER_ID, nome.c_str(), cognome.c_str());
    try
    {
        res = db.ExecSQLcmd(comando);
        std::string conferma = "Corriere inserito nel database!\n";
        send(clientSocket, conferma.c_str(), conferma.length(), 0);
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
