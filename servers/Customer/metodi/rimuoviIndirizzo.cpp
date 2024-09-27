#include "rimuoviIndirizzo.h"

bool rimuoviIndirizzo(int addressID)
{
    char comando[1000];
    PGresult *res;
    Con2DB db(HOSTNAME, DB_PORT, USERNAME_CUST, PASSWORD_CUST, DB_NAME); // Connessione DB
    try
    {
	    sprintf(comando, "DELETE FROM Indirizzo WHERE id = %d", addressID);
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