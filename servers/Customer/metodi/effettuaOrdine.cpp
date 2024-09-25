#include "effettuaOrdine.h"

bool effettuaOrdine(int customerID, const char* pagamento, int indirizzo)
{
    char comando[1000];
    PGresult *res;
    Con2DB db(HOSTNAME, DB_PORT, USERNAME_CUST, PASSWORD_CUST, DB_NAME); // Effettua la connessione al database
    try
    {
        //Inserisce l'ordine nel database
        sprintf(comando, "INSERT INTO ordine(customer, datarich, pagamento, indirizzo) VALUES (%d, NOW(), '%s', %d) RETURNING id",
        customerID, pagamento, indirizzo);
        res = db.ExecSQLcmd(comando);
        PQclear(res);
        // Ogni prodotto viene inserito nell'ordine tramite funzione del db e il carrello viene svuotato
        return true;
    }
    catch(...)
    {
        return false;
    }
}