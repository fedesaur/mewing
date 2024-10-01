#include "modificaFornito.h"

bool modificaFornito(const char* nomeProdotto, const char* descrizioneProdotto, double prezzoProdotto, int productID)
{
    PGresult *res;
    char comando[1000];
    Con2DB db(HOSTNAME, DB_PORT, USERNAMEP, PASSWORDP, DB_NAME);
    try
    {
        sprintf(comando, "UPDATE prodotto SET descrizione = '%s', prezzo = %f , nome = '%s' WHERE id = %d",
        descrizioneProdotto, prezzoProdotto, nomeProdotto, productID);
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
