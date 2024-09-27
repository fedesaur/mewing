#include "autenticazione.h"

int autentica(const char* IVA)
{
	PGresult *res;
    char comando[1000];
    int rows;
	int ID;
	Con2DB db(HOSTNAME, DB_PORT, USERNAME, PASSWORD, DB_NAME); // Effettua la connessione al database
	
	try
	{
		// Recupera il fornitore tramite l'email
		sprintf(comando, "SELECT id FROM trasportatore WHERE piva = '%s' ", IVA);
    	res = db.ExecSQLtuples(comando);
    	rows = PQntuples(res);
		ID = 0;
    	if (rows > 0) // Se viene trovato un utente con quella mail...
    	{
			//...vengono recuperati i suoi dati ed inviati al server tramite Redis
        	ID = atoi(PQgetvalue(res, 0, PQfnumber(res, "id")));
    	}
	}
	catch(...)
	{
		ID = -1;
	}
	PQclear(res);
	return ID;
}

bool crea(const char* piva, const char* nome, const char* via, int civico, const char* CAP, const char* city, const char* stato)
{
	PGresult *res;
	char comando[1000];
	Con2DB db(HOSTNAME, DB_PORT, USERNAME, PASSWORD, DB_NAME); // Effettua la connessione al database
	try
    {
	    sprintf(comando, "INSERT INTO Indirizzo(via, civico, cap, citta, stato) VALUES('%s', %d, '%s', '%s', '%s') RETURNING id", via, civico, CAP, city, stato);
	    res = db.ExecSQLtuples(comando);
        int address = atoi(PQgetvalue(res, 0, PQfnumber(res, "id"))); // Recupera l'ID dell'indirizzo appena aggiunto
        sprintf(comando, "INSERT INTO trasportatore(piva, nome, indirizzo) VALUES ('%s', '%s', %d)", piva, nome, address);
		res = db.ExecSQLcmd(comando);
		PQclear(res);
		return true;
    }
    catch(...) //Se c'è stato un errore, lo segnala all'utente
    {  
        PQclear(res);
        return false;
    }
}
