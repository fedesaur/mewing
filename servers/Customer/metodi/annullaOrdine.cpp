#include "annullaOrdine.h"

bool annullaOrdine(int IDOrdine, int userID)
{
    char comando[1000];
    int rows;
    PGresult *res;
    Con2DB db(HOSTNAME, DB_PORT, USERNAME_CUST, PASSWORD_CUST, DB_NAME); // Connessione DB
    try
    {
        sprintf(comando, "SELECT * FROM ordine WHERE id = %d", IDOrdine);
        res = db.ExecSQLtuples(comando);
        if (PQntuples(res) == 0) return false; // Se non ci sono ordini con quell'ID, impedisce l'operazione
        PQclear(res);

        sprintf(comando, "SELECT * FROM transord WHERE ordine = %d", IDOrdine);
        res = db.ExecSQLtuples(comando);
        if (PQntuples(res) > 0) return false; //Se l'ordine è già stato preso in carico, non si può annullare
        PQclear(res);
        
        // Annulla l'ordine
        sprintf(comando, "UPDATE ordine SET stato = 'annullato' WHERE id = %d AND customer = %d", IDOrdine, userID);
        res = db.ExecSQLcmd(comando);
        PQclear(res);
        return true;
    }
    catch(...)
    {
        // Gestione errori database
        return false;
    }
}
