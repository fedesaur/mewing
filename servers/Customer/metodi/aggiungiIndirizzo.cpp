#include "aggiungiIndirizzo.h"

bool aggiungiindirizzo( int userID,  const char* via, int civico, const char* CAP, const char* city, const char* stato)
{
    char comando[1000];
    PGresult *res;
    Con2DB db(HOSTNAME, DB_PORT, USERNAME_CUST, PASSWORD_CUST, DB_NAME); // Connessione DB
    try
    {
	    sprintf(comando, "INSERT INTO Indirizzo(via, civico, cap, citta, stato) VALUES('%s', %d, '%s', '%s', '%s') RETURNING id", via, civico, CAP, city, stato);
	    res = db.ExecSQLtuples(comando);
        int address = atoi(PQgetvalue(res, 0, PQfnumber(res, "id"))); // Recupera l'ID dell'indirizzo appena aggiunto
        sprintf(comando, "INSERT INTO custadd(customer, addr) VALUES (%d, %d)", userID, address);
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
