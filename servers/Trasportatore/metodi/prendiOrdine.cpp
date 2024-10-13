#include "prendiOrdine.h"

bool prendiOrdine(int courierID, int corriere, int ordine)
{
    int rows;
    char comando[1000];
    int RIGHE_CORRIERI;
    int RIGHE_ORDINI;
    PGresult *res;
    Con2DB db(HOSTNAME, DB_PORT, USERNAME_TRAS, PASSWORD_TRAS, DB_NAME); // Effettua la connessione al database
    try
    {
        // Prima di prendere in carico un ordine, controlla che un altro trasportatore non l'abbia già preso in carico
        sprintf(comando, "SELECT * FROM ordine WHERE id = %d", ordine);
        res = db.ExecSQLtuples(comando);
        rows = PQntuples(res);
        if (rows == 0) return false; // Se l'ordine non esiste, impedisce l'operazione
        PQclear(res); //Altrimenti la permette
        
        sprintf(comando, "SELECT * FROM transord WHERE ordine = %d", ordine);
        res = db.ExecSQLtuples(comando);
        rows = PQntuples(res);
        if (rows > 0) return false; // Se qualcuno l'ha già preso in carico, impedisce l'operazione
        PQclear(res); //Altrimenti la permette

        
        
        sprintf(comando, "SELECT * FROM transord WHERE ordine = %d AND trasportatore = %d", ordine, courierID);
        res = db.ExecSQLtuples(comando);
        rows = PQntuples(res);
        if (rows > 0) return false; // Se il trasportatore ha già preso in carico l'ordine, impedisce di prenderlo in carico di nuovo
        PQclear(res); //Altrimenti la permette
        
        // Prende in carico l'ordine
        sprintf(comando, "INSERT INTO transord(ordine, trasportatore) VALUES(%d, %d)", ordine, courierID);
        res = db.ExecSQLcmd(comando);
        PQclear(res);
        
        // Assegna l'ordine ad un corriere
        sprintf(comando, "INSERT INTO consegna(ordine, corriere) VALUES (%d, %d)", ordine, corriere);
        res = db.ExecSQLcmd(comando);
        PQclear(res);
        
        // Aggiorna l'ordine indicando che è stato preso in carico
        sprintf(comando, "UPDATE ordine SET stato = 'accettato' WHERE id = %d", ordine);
        res = db.ExecSQLcmd(comando);
        PQclear(res);
        return true;
    }
    catch(...)
    {
        return false;
    }
}