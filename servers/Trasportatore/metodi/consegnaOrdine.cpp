#include "consegnaOrdine.h"

bool consegnaOrdine(int ordineID)
{
    char comando[1000];
    PGresult *res;
    Con2DB db(HOSTNAME, DB_PORT, USERNAME_TRAS, PASSWORD_TRAS, DB_NAME); // Effettua la connessione al database
    try
    {
        sprintf(comando, "INSERT INTO ordineconse(id, datacons) VALUES (%d, NOW())", ordineID);
        res = db.ExecSQLcmd(comando);
        PQclear(res);
        return true; 
    }
    catch(...)
    {
        PQclear(res);
        return false;
    }
    
}