#include "rimuoviFornito.h"

bool rimuoviFornito(int supplierID, int productID)
{
    char comando[1000];
    PGresult *res;

    Con2DB db(HOSTNAME, DB_PORT, USERNAMEP, PASSWORDP, DB_NAME);
    try
    {
        sprintf(comando, "SELECT * FROM prodotto WHERE id = %d AND fornitore = %d", productID, supplierID);
        res = db.ExecSQLtuples(comando);
        if (PQntuples(res) == 0) return false; // Se non ci sono prodotti con l'ID specificato, impedisce l'operazione
        PQclear(res);

        sprintf(comando, "DELETE FROM prodotto WHERE id = %d AND fornitore = %d", productID, supplierID);
        res = db.ExecSQLcmd(comando);
        PQclear(res);
        return true;
    }
    catch(...)
    {
        PQclear(res);
        // Se ci sono errori nella query, vengono catturati da catch
        return false;
    }
    return true;
}
