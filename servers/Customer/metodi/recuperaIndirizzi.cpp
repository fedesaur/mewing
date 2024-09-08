#include "recuperaIndirizzi.h"

std::pair<int, Indirizzo*> recuperaIndirizzi(int clientSocket)
{
    std::pair <int, Indirizzo*> risultato;
    int RIGHE;
    int CUSTOMER_ID;
    char comando[1000];
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
    CUSTOMER_ID = atoi(id.c_str()); // ID Customer
    sprintf(comando, "SELECT ind.id, ind.via, ind.civico, ind.cap, ind.citta, ind.stato "
    "FROM indirizzo ind, custadd cst WHERE cst.customer = %d AND cst.add = ind.id", CUSTOMER_ID);
    try
    {
        RIGHE = PQntuples(res);
        if (RIGHE > 0)
        {
            Indirizzo* indirizzi = new Indirizzo[RIGHE];
            // Recupera gli indirizzi e li memorizza
            for (int i = 0; i < RIGHE; i++)
            {
                // Recupera gli attributi degli dalla query sopra svolta...
                int ID = atoi(PQgetvalue(res, i, PQfnumber(res, "id")));
                const char* via = PQgetvalue(res, i, PQfnumber(res, "via"));
                int civico = atoi(PQgetvalue(res, i, PQfnumber(res, "civico")));
                const char* CAP = PQgetvalue(res, i, PQfnumber(res, "cap"));
                const char* citta = PQgetvalue(res, i, PQfnumber(res, "citta"));
                const char* stato = PQgetvalue(res, i, PQfnumber(res, "stato"));

                // e li assegna all'i-esimo Indirizzo in indirizzi
                indirizzi[i].ID = ID;
                indirizzi[i].via = via;
                indirizzi[i].civico = civico;
                indirizzi[i].CAP = CAP;
                indirizzi[i].citta = citta;
                indirizzi[i].stato = stato;
            }
            risultato.first = RIGHE; // Ritorna il numero di righe degli indirizzi
            risultato.second = indirizzi; // Ritorna l'array degli indirizzi
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
	    std::string errore = "C'è stato un errore nel database\n\n"; // Seleziona la frase del turno
	    send(clientSocket, errore.c_str(), errore.length(), 0); // Invia il messaggio pre-impostato all'utente
        return risultato;
    }
}

void mostraIndirizzi(int clientSocket, int righe, Indirizzo* indirizzi)
{
    if (righe > 0)
    {
        std::string request = "\nINDIRIZZI REGISTRATI:\n";
	    send(clientSocket, request.c_str(), request.length(), 0); // Invia il messaggio pre-impostato all'utente
        for (int i = 0; i < righe; i++)
        {
            std::string indirizzo = std::to_string(i+1) + ") ID Indirizzo: " + std::to_string(indirizzi[i].ID) +
             " Via: " + indirizzi[i].via + 
             " Civico: " + std::to_string(indirizzi[i].civico) + 
             " CAP: " + indirizzi[i].CAP + 
             " Città: " + indirizzi[i].citta + 
             " Stato :" + indirizzi[i].stato  + "\n";
	        send(clientSocket, indirizzo.c_str(), indirizzo.length(), 0);
        }
    } else {
        std::string request = "Non ci sono indirizzi registrati!\n\n"; //... e lo stampa
	    send(clientSocket, request.c_str(), request.length(), 0); // Invia il messaggio pre-impostato all'utente
    }
    return;
}