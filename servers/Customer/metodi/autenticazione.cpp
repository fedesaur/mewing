#include "autenticazione.h"

int autentica(const char* mail)
{
	redisContext *c2r;
	redisReply* reply;
	PGresult *res;
    char comando[1000];
    int rows;
	int ID;
	Con2DB db(HOSTNAME, DB_PORT, USERNAME, PASSWORD, DB_NAME); // Effettua la connessione al database
	
	// Effettua la connessione a Redis
	c2r = redisConnect(REDIS_IP, REDIS_PORT);
	try
	{
		// Recupera il fornitore tramite l'email
		sprintf(comando, "SELECT id FROM customers WHERE mail = '%s' ", mail);
    	res = db.ExecSQLtuples(comando);
    	rows = PQntuples(res);
		ID = 0;
    	if (rows > 0) // Se viene trovato un utente con quella mail...
    	{
			//...vengono recuperati i suoi dati ed inviati al server tramite Redis
        	ID = atoi(PQgetvalue(res, 0, PQfnumber(res, "id")));
			reply = RedisCommand(c2r, "XADD %s * %s %d", WRITE_STREAM, mail, ID);
        	assertReplyType(c2r, reply, REDIS_REPLY_STRING);
        	freeReplyObject(reply);
    	}
	}
	catch(...)
	{
		ID = -1;
	}
	PQclear(res);
	return ID;
}

bool creaCustomer(const char* email, const char* nome, const char* cognome, const char* via, int civico, const char* CAP, const char* city, const char* stato)
{
	PGresult *res;
	char comando[1000];
	Con2DB db(HOSTNAME, DB_PORT, USERNAME, PASSWORD, DB_NAME); // Effettua la connessione al database
	try
	{
		// Viene inserito il nuovo indirizzo nel database
		sprintf(comando, "INSERT INTO Indirizzo(via, civico, cap, citta, stato) VALUES('%s', %d, '%s', '%s', '%s') RETURNING id", via, civico, CAP, city, stato);
		res = db.ExecSQLtuples(comando); 
		int abita = atoi(PQgetvalue(res, 0, PQfnumber(res, "id"))); // Recupera l'ID dell'indirizzo appena aggiunto
		
		sprintf(comando, "INSERT INTO customers(nome, cognome, mail, abita) VALUES('%s', '%s', '%s', %d)",
		nome, cognome, email, abita);
		res = db.ExecSQLcmd(comando); // Viene inserito il nuovo fornitore nel database nel database
		PQclear(res);	
		return true;
	}
	catch(...)
	{
		return false;
	}
}
