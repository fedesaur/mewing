#include "autenticazione.h"

bool autentica(int clientSocket)
{
	redisContext *c2r; // c2r contiene le info sul contesto
	redisReply *reply; // reply contiene le risposte da Redis

	c2r = redisConnect(REDIS_IP, REDIS_PORT); // Effettua la connessione a Redis
	//Con2DB db(HOSTNAME, DB_PORT, USERNAME, PASSWORD, DB_NAME); // Effettua la connessione al database
	/*
  	 Con2DB(const char *hostname,
	 const char *port,
	 const char *username,
	 const char *password,
	 const char *dbname);
	*/

	// Legge l'ultima mail nello stream
    reply = RedisCommand(c2r, "XREVRANGE %s + - COUNT 1", CUSTOMER_STREAM);
    if (reply == nullptr || reply->type != REDIS_REPLY_ARRAY || reply->elements == 0)
	{
       std::cerr << "Errore nel comando Redis o stream vuoto" << std::endl;
       return false;
    }

    redisReply* stream = reply -> element[0];
    redisReply* entryFields = stream -> element[1];
    std::string fieldName = entryFields->element[0]->str; // Chiave
   	std::string received_email = entryFields->element[1]->str; // Valore
	freeReplyObject(reply);

	if (received_email.empty())
	{
		std::cerr << "Errore: non è stata trovata nessuna email con la chiave specificata." << std::endl;
		return false;
	}
	std::cout << "Email letta dallo stream: " << received_email << std::endl;
	const char* mail = received_email.c_str();
	bool esiste = controllaEsistenza(db, mail); // Controlla se esiste un Customer con quella mail
	std::cout << "Risultato query: " << esiste << std::endl;
	return true;
}

bool controllaEsistenza(Con2DB db, const char* mail)
{
	PGresult *res;
	char comando[1000];

	// sprintf si occupa di creare una stringa con una data formattazione
	sprintf(comando,"SELECT * FROM CUSTOMERS WHERE mail = %s", mail);

	//res = db.ExecSQLtuples(comando); //Esegue la query sopra citata
	int rows;
	//rows = PQntuples(res);
	//PQclear(res);
	//std::cout << rows << std::endl;
	return rows == 0;
}
