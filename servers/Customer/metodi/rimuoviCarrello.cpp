#include "rimuoviCarrello.h"

bool rimuoviCarrello(int idProdotto, int userID)
{
    char comando[1000];
    PGresult *res;
    Con2DB db(HOSTNAME, DB_PORT, USERNAME_CUST, PASSWORD_CUST, DB_NAME); // Connessione DB
    try
    {
        sprintf(comando, "DELETE FROM prodincart WHERE carrello = %d AND prodotto = %d", userID, idProdotto);
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