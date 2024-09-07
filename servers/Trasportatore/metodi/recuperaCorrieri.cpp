#include "registraCorriere.h"

std::pair<int, Corriere*> recuperaCorrieri(int clientSocket)
{
    int COURIER_ID;
    char comando[1000];
    std::pair<int,Corriere*> risultato;
    PGresult *res;
    redisContext *c2r; // c2r contiene le info sul contesto
	redisReply *reply; // reply contiene le risposte da Redis

	c2r = redisConnect(REDIS_IP, REDIS_PORT); // Effettua la connessione a Redis
	Con2DB db(HOSTNAME, DB_PORT, USERNAME, PASSWORD, DB_NAME); // Effettua la connessione al database

    reply = RedisCommand(c2r, "XREVRANGE %s + - COUNT 1", READ_STREAM);
    if (reply == nullptr || reply->type != REDIS_REPLY_ARRAY || reply->elements == 0)
    {
        std::cerr << "Errore nel comando Redis o stream vuoto" << std::endl;
    }
    std::string id = reply->element[0]->element[1]->element[1]->str; 
    COURIER_ID = atoi(id.c_str()); // ID Corriere

    try
    {
        sprintf(comando, "SELECT id, nome, cognome FROM corriere WHERE azienda = %d", COURIER_ID);        
        res = db.ExecSQLtuples(comando)
        int RIGHE = PQntuples(res);
        if (RIGHE > 0)
        {
            Corriere* corrieriRegistrati = new Corriere[RIGHE];
            for (int i = 0; i < RIGHE; i++)
            {
                int ID = atoi(PQgetvalue(res, i, PQfnumber(res, "id")));
                const char* nome = PQgetvalue(res, i, PQfnumber(res, "nome"));
                const char* cognome = PQgetvalue(res, i, PQfnumber(res, "cognome"));

                corrieriRegistrati[i].ID = ID;
                corrieriRegistrati[i].nome = nome;
                corrieriRegistrati[i].cognome = cognome;
            }
            risultato.first = RIGHE;
            risultato.second = corrieriRegistrati;
        } else {
            std::string vuoto = "Non ci sono corrieri registrati!\n";
		    send(clientSocket, vuoto.c_str(), vuoto.length(), 0); // Invia il messaggio pre-impostato all'utente
            risultato.first = 0;
            risultato.second = nullptr;
        }
    }
    catch(...)
    {
        std::string errore = "C'è stato un errore nel database\n";
		send(clientSocket, errore.c_str(), errore.length(), 0); // Invia il messaggio pre-impostato all'utente
        risultato.first = -1;
        risultato.second = nullptr;
    }
    PQclear(res);
    return risultato;
}

void mostraCorrieri(int clientSocket, int righe, Corriere* corrieri)
{
    if (righe > 0)
    {
        std::string request = "\nCORRIERI REGISTRATI:\n"; //... e lo stampa
	    send(clientSocket, request.c_str(), request.length(), 0); // Invia il messaggio pre-impostato all'utente
        for (int i = 0; i < righe; i++)
        {
            // Recupera gli attributi dei prodotti dal carrello...
            int ID = corrieri[i].ID;
            const char* nome = corrieri[i].nome;
            const char* cognome = corrieri[i].cognome;
            // ...e li invia all'utente così che possa visualizzarli ed effettuarci operazioni
            std::string corr = std::to_string(i+1) + ") ID Corriere: " + std::to_string(ID) +
             " Nome Corriere: " + nome + 
             " Cognome Corriere: " + cognome + "\n";
	        send(clientSocket, corr.c_str(), corr.length(), 0);
        }
    } else {
        std::string request = "Non ci sono corrieri registrati!\n"; //... e lo stampa
	    send(clientSocket, request.c_str(), request.length(), 0); // Invia il messaggio pre-impostato all'utente
    }
    return;
}