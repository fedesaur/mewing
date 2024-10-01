#include "modificaInfoF.h"

bool modificaInfoF(const char* email, const char* nome, const char* IVA, const char* telefono)
{
    PGresult *res;
    char comando[1000];
    Con2DB db(HOSTNAME, DB_PORT, USERNAMEP, PASSWORDP, DB_NAME);
    try
    {
        sprintf(comando,"UPDATE fornitore SET nome = '%s', telefono = '%s' , piva = '%s' WHERE mail = '%s' ", nome, telefono, IVA, email);
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
