#include "modificaInfoF.h"

bool modificaInfoF(const char* email, const char* nome, const char* IVA, const char* telefono)
{
    PGresult *res;
    char comando[1000];
    Con2DB db(HOSTNAME, DB_PORT, USERNAMEP, PASSWORDP, DB_NAME);
    try
    {   
        sprintf(comando, "SELECT * FROM fornitore WHERE piva = '%s' AND mail != '%s'", IVA, email);
        res = db.ExecSQLtuples(comando);
        if (PQntuples(res) > 0) return false; // Se vi è un altro utente con la stessa partita IVA, impedisce l'operazione
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
