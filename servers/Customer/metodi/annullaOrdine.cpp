#include "annullaOrdine.h"

bool annullaOrdine(int IDOrdine, int userID)
{
    char comando[1000];
    int rows;
    PGresult *res;
    Con2DB db(HOSTNAME, DB_PORT, USERNAME_CUST, PASSWORD_CUST, DB_NAME); // Connessione DB
    try
    {
        sprintf(comando, "SELECT * FROM transord WHERE ordine = %d", IDOrdine;
        res = db.ExecSQLtuples(comando);
        rows = PQntuples(res);
        if (rows >= 1)
        {
            PQclear(res);
            return false;
        }
        PQclear(res);
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
