#include "rimuoviCorriere.h"

bool rimuoviCorriere(int trasporterID, int courierID)
{
    char comando[1000];
    PGresult *res;

	Con2DB db(HOSTNAME, DB_PORT, USERNAME_TRAS, PASSWORD_TRAS, DB_NAME); // Effettua la connessione al database
    // Ricevuti i dati, aggiunge il Corriere al database
    
    try
    {
        sprintf(comando, "SELECT * FROM corriere WHERE id = %d AND azienda = %d", courierID, trasporterID);
        res = db.ExecSQLtuples(comando);
        if (PQntuples(res) == 0) return false;  // Se il corriere non è nel sistema, impedisce l'operazione
        
        PQclear(res);
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
