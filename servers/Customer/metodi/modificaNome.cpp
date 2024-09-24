#include "modificaNome.h"

bool modificaInfoCustomer(const char* email, const char* nome, const char* cognome)
{
    int USER_ID;
    PGresult *res;
    char comando[1000];
    Con2DB db(HOSTNAME, DB_PORT, USERNAME_CUST, PASSWORD_CUST, DB_NAME);

    try
    {
        sprintf(comando, "UPDATE customers SET nome = '%s', cognome = '%s' WHERE mail = '%s' ", nome, cognome, email);
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
