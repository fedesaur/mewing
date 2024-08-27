#include "autenticazione.h"

bool autentica(int clientSocket)
{
	redisContext *c2r; // c2r contiene le info sul contesto
	redisReply *reply; // reply contiene le risposte da Redis

	c2r = redisConnect(REDIS_IP, REDIS_PORT); // Effettua la connessione a Redis
	Con2DB db(HOSTNAME, DB_PORT, USERNAME, PASSWORD, DB_NAME); // Effettua la connessione al database
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
	std::cout << "Risultato query: " << esiste << std::endl; // 0 = Non esiste 1 = Esiste
	//if (esiste)
	return true;
	//return recuperaCustomer(db, clientSocket, mail); // Il customer esiste e viene ritornato
	//return creaCustomer(db, clientSocket, mail); // Il customer non esiste e viene creato
}

bool controllaEsistenza(Con2DB db, const char* mail)
{
	PGresult *res;
	char comando[1000];
	int rows;

	// sprintf si occupa di creare una stringa con una data formattazione
	sprintf(comando, "SELECT * FROM customers WHERE mail = \'%s\'", mail);
	res = db.ExecSQLtuples(comando); //Esegue la query sopra citata
	rows = PQntuples(res);
	if (rows == 0)  //L'utente non esiste
	{
		PQclear(res);
		return false;
	}
	// L'utente esiste e ne stampo i dati
	std::cout << "ID utente: " << atoi(PQgetvalue(res, 0, PQfnumber(res, "id"))) << std::endl;
	std::cout << "Nome utente: " << PQgetvalue(res, 0, PQfnumber(res, "nome")) << std::endl;
	std::cout << "Cognome utente: " << PQgetvalue(res, 0, PQfnumber(res, "cognome")) << std::endl;
	PQclear(res);
	return true; // 0 = Non esiste 1 = Esiste
}

bool creaCustomer(Con2DB db, int clientSocket, const char* mail)
{
	int datiRichiesti = 3;
	int datiRicevuti = 0;
	char buffer[1024] = {0};
	// Chiede i dati neccessari per creare il customer (nome, cognome, abita)
	while (datiRicevuti < datiRichiesti)
	{

	}

	std::string request = "Inserisci la tua email\n";
	send(clientSocket, request.c_str(), request.length(), 0);

	int bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
	if (bytesRead > 0) {
		//Chide all'utente una mail utile all'identificazione
		std::string email(buffer, bytesRead);
		std::string response = "Email ricevuta! Procedo all'autenticazione\n";
		send(clientSocket, response.c_str(), response.length(), 0);

		return autentica(clientSocket); // Passa al processo di autenticazione
	}
	std::cerr << "Errore o nessun dato ricevuto dal client." << std::endl;
	return false;
}

bool recuperaCustomer(Con2DB db, int clientSocket, const char* mail)
{
	PGresult *res;
	char comando[1000];

	// sprintf si occupa di creare una stringa con una data formattazione
	sprintf(comando, "SELECT * FROM customers WHERE mail = '%s'", mail);
	res = db.ExecSQLtuples(comando); //Esegue la query sopra citata
	std::cout << "Risultato query: " << std::endl;
	PQclear(res);
	return false; // 0 = Non esiste 1 = Esiste
}
