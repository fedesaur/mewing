#include "aggiungiCarrello.h"

bool aggiungiCarrello(int idProdotto, int userID, int quantita)
{
    char comando[1000];
    PGresult *res;
    Con2DB db(HOSTNAME, DB_PORT, USERNAME_CUST, PASSWORD_CUST, DB_NAME); // Connessione DB
    try
    {
        sprintf(comando, "INSERT INTO prodincart(carrello, prodotto, quantita) VALUES (%d, %d, %d)", userID, idProdotto, quantita);
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
