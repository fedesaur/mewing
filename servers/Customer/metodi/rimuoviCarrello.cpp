#include "rimuoviCarrello.h"

bool rimuoviCarrello(int idProdotto, int userID)
{
    char comando[1000];
    PGresult *res;
    Con2DB db(HOSTNAME, DB_PORT, USERNAME_CUST, PASSWORD_CUST, DB_NAME); // Connessione DB
    try
    {
        sprintf(comando, "SELECT * FROM prodincart WHERE carrello = %d AND prodotto = %d", userID, idProdotto);
        res = db.ExecSQLtuples(comando);
        if (PQntuples(res) == 0) return false; // Se non ci sono prodotti nel carrello con quell'ID, impedisce l'operazione
        PQclear(res);
        // Rimuove il prodotto dal carrello
        sprintf(comando, "DELETE FROM prodincart WHERE carrello = %d AND prodotto = %d", userID, idProdotto);
        res = db.ExecSQLcmd(comando);
        PQclear(res);
        return true;
    }
    catch(...)
    {
        // Gestione errori database
        PQclear(res);
        return false;
    }
}