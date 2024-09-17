#include "rimuoviCorriere.h"

bool rimuoviCorriere(int clientSocket, int courierID)
{
    int TRASPORTER_ID;
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
    TRASPORTER_ID = atoi(id.c_str()); // ID Trasportatore
    // Ricevuti i dati, aggiunge il Corriere al database
    sprintf(comando, "DELETE FROM corriere WHERE id = %d AND azienda = %d", courierID, TRASPORTER_ID);
    try
    {
        res = db.ExecSQLcmd(comando);
        std::string conferma = "Corriere rimosso dal database!\n";
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
