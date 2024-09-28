#include "registraCorriere.h"

bool registraCorriere(int courierID, const char* nome, const char* cognome)
{
    char comando[1000];
    PGresult *res;
	Con2DB db(HOSTNAME, DB_PORT, USERNAME_TRAS, PASSWORD_TRAS, DB_NAME); // Effettua la connessione al database
    // Ricevuti i dati, aggiunge il Corriere al database
    try
    {
        sprintf(comando, "INSERT INTO corriere(azienda, nome, cognome) VALUES (%d, '%s', '%s')", courierID, nome, cognome);
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
