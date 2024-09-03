#include "aggiungiProdotto.h"

bool aggiungiProdotto(int clientSocket)
{
    int USER_ID;
    PGresult *res;
    redisContext *c2r; // c2r contiene le info sul contesto
	redisReply *reply; // reply contiene le risposte da Redis
    char comando[1000];
    char buffer[1024] = {0};
    int DATI_NECESSARI = 3;
    std::string FRASI[] = {"Nome del prodotto:\n", "Descrizione del prodotto:\n", "Prezzo del prodotto\n"};

	c2r = redisConnect(REDIS_IP, REDIS_PORT); // Effettua la connessione a Redis
	Con2DB db(HOSTNAME, DB_PORT, USERNAME, PASSWORD, DB_NAME); // Effettua la connessione al database

    reply = RedisCommand(c2r, "XREVRANGE %s + - COUNT 1", READ_STREAM);
    if (reply == nullptr || reply->type != REDIS_REPLY_ARRAY || reply->elements == 0)
    {
        std::cerr << "Errore nel comando Redis o stream vuoto" << std::endl;
        return false;
    }
    int datiRichiesti = 0;
    std::string nomeProdotto;
    std::string descrizioneProdotto;
    double prezzo;
    
    while (datiRichiesti < DATI_NECESSARI)
    {
		std::string request = FRASI[datiRichiesti]; // Seleziona la frase del turno
		send(clientSocket, request.c_str(), request.length(), 0); // Invia il messaggio pre-impostato all'utente
		bool attendiInput = true;
        while (attendiInput)
        {
            int bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0); 
		    if (bytesRead > 0)
		    {
			// bool correctInput = true; Nel caso i dati non vadano bene, si potrebbe pensare a ripetere quel passaggio
			std::cout << buffer;
			switch(datiRichiesti)
			{
				case 0:
					std::string temp = buffer.pop_back();
                    if (temp.length() > 100)
                    {
                        std::string errore = "La lunghezza del nome deve essere di massimo 100 caratteri!\n"; //... e lo stampa
	                    send(clientSocket, errore.c_str(), errore.length(), 0); // Invia il messaggio pre-impostato all'utente
                        break;
                    }
					nomeProdotto = temp;
					datiRichiesti++;
                    break;
				case 1:
					descrizioneProdotto = buffer.pop_back();
                    datiRichiesti++;
					break;
				case 2:
					prezzo = atof(buffer);
                    datiRichiesti++;
					break;
			}
		    } else {
                std::string errore = "Errore nella ricezione dei dati!\n";
	            send(clientSocket, errore.c_str(), errore.length(), 0); // Invia il messaggio pre-impostato all'utente
            }
	    }
    }
    // Viene inserito il nuovo indirizzo nel database
	sprintf(comando, "INSERT INTO prodotto(descrizione, prezzo, nome) VALUES('%s', %f, '%s')", descrizioneProdotto.c_str(), prezzo, nomeProdotto.c_str());
	res = db.ExecSQLtuples(comando); 
	if (PQresultStatus(res) != PGRES_TUPLES_OK)  // Controlla che la query sia andata a buon fine
    {
        std::string errore = "Errore nel database!\n";
	    send(clientSocket, errore.c_str(), errore.length(), 0);  // Invia il messaggio pre-impostato all'utente
        return false;
    } 
    std::string successo = "Prodotto aggiunto correttamente!\n";
    send(clientSocket, successo.c_str(), successo.length(), 0); // Invia il messaggio pre-impostato all'utente
    return true;
}