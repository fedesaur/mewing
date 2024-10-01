#include "aggiungiFornito.h"

bool aggiungiFornito(int supplierID, const char* nomeProdotto, const char* descrizioneProdotto, double prezzoProdotto)
{
    PGresult *res;
    char comando[1000];
	Con2DB db(HOSTNAME, DB_PORT, USERNAMEP, PASSWORDP, DB_NAME);
    try
    {
        sprintf(comando, "INSERT INTO prodotto(descrizione, prezzo, nome, fornitore) VALUES('%s', %f, '%s', %d)",
        descrizioneProdotto, prezzoProdotto, nomeProdotto, supplierID);
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
