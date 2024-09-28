#include "rimuoviCorriere.h"

bool rimuoviCorriere(int trasporterID, int courierID)
{
    char comando[1000];
    PGresult *res;

	Con2DB db(HOSTNAME, DB_PORT, USERNAME_TRAS, PASSWORD_TRAS, DB_NAME); // Effettua la connessione al database
    // Ricevuti i dati, aggiunge il Corriere al database
    
    try
    {
        sprintf(comando, "DELETE FROM corriere WHERE id = %d AND azienda = %d", courierID, trasporterID);
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
