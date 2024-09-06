#include "aggiungiFornito.h"

bool aggiungiFornito(int clientSocket)
{
    int PRODUCER_ID;
    PGresult *res;
    redisContext *c2r;
	redisReply *reply;
    char comando[1000];
    char buffer[1024] = {0};
    int DATI_NECESSARI = 3;
    std::string FRASI[] = {"Nome del prodotto:\n", "Descrizione del prodotto:\n", "Prezzo del prodotto\n"};

	c2r = redisConnect(REDIS_IP, REDIS_PORT);
	if (c2r == nullptr || c2r->err) {
        std::cerr << "Errore di connessione a Redis: " << (c2r ? c2r->errstr : "Errore sconosciuto") << std::endl;
        return false;
    }
    
	Con2DB db(HOSTNAME, DB_PORT, USERNAMEP, PASSWORDP, DB_NAME);

    reply = RedisCommand(c2r, "XREVRANGE %s + - COUNT 1", READ_STREAM);
    if (reply == nullptr || reply->type != REDIS_REPLY_ARRAY || reply->elements == 0)
    {
        std::cerr << "Errore nel comando Redis o stream vuoto" << std::endl;
        return false;
    }

    std::string id = reply->element[0]->element[1]->element[1]->str; 
    PRODUCER_ID = stoi(id);

    int datiRichiesti = 0;
    std::string temp;
    std::string nomeProdotto;
    std::string descrizioneProdotto;
    double prezzo;
    
    while (datiRichiesti < DATI_NECESSARI)
    {
		std::string request = FRASI[datiRichiesti];
		send(clientSocket, request.c_str(), request.length(), 0);
        int bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0); 
		if (bytesRead > 0)
		{
			switch(datiRichiesti)
			{
			    case 0:
					temp = buffer;
                    temp.pop_back();
                    if (temp.length() > 100)
                    {
                        std::string errore = "La lunghezza del nome deve essere di massimo 100 caratteri!\n";
	                    send(clientSocket, errore.c_str(), errore.length(), 0); 
                    } else {
                        nomeProdotto = temp;
					    datiRichiesti++;   
                    }
					break;
				case 1:
					temp = buffer;
                    temp.pop_back();
                    descrizioneProdotto = temp;
                    datiRichiesti++;
					break;
				case 2:
					prezzo = atof(buffer);
                    if (prezzo <= 0) { // Aggiunge validazione del prezzo
                        std::string errore = "Prezzo non valido!\n";
	                    send(clientSocket, errore.c_str(), errore.length(), 0);
                    } else {datiRichiesti++;}
					break;
			}
		} else {
            std::cerr << "Errore nella ricezione dei dati. Codice di errore: " << errno << std::endl;
            std::string errore = "Errore nella ricezione dei dati!\n";
	        send(clientSocket, errore.c_str(), errore.length(), 0); 
	    }
    }

	sprintf(comando, "INSERT INTO prodotto(descrizione, prezzo, nome, fornitore) VALUES('%s', %f, '%s', %d)",
    descrizioneProdotto.c_str(), prezzo, nomeProdotto.c_str(), PRODUCER_ID);
    try
    {
        res = db.ExecSQLcmd(comando);
        std::string successo = "Prodotto aggiunto correttamente!\n";
        send(clientSocket, successo.c_str(), successo.length(), 0);
        return true;
    }
    catch(...)
    {
        std::string errore = "Errore nel database!\n";
	    send(clientSocket, errore.c_str(), errore.length(), 0);
        return false;
    }
}
