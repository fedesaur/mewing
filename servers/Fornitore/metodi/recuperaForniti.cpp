#include "recuperaForniti.h"

std::pair<int, Prodotto*> recuperaForniti(const char* mail)
{
    std::pair <int, Prodotto*> risultato;
    int rows;
    int PRODUCER_ID;
    char comando[1000];
    PGresult *res;
	Con2DB db(HOSTNAME, DB_PORT, USERNAMEP, PASSWORDP, DB_NAME); // Effettua la connessione al database

    try
    {
        sprintf(comando, "SELECT pr.id, pr.descrizione, pr.prezzo, pr.nome FROM prodotto pr, fornitore, fr WHERE pr.fornitore = fr.id AND fr.mail =  '%s' ", mail);
        res = db.ExecSQLtuples(comando);
        rows = PQntuples(res);
        if (rows > 0)
        {
            Prodotto* forniti = new Prodotto[rows];
            // Recupera i prodotti e li memorizza in forniti
            for (int i = 0; i < rows; i++)
            {
            // Recupera gli attributi dei prodotti dalla query sopra svolta...
            int ID = atoi(PQgetvalue(res, i, PQfnumber(res, "id")));
            const char* descrizione = PQgetvalue(res, i, PQfnumber(res, "descrizione"));
            double prezzo = atof(PQgetvalue(res, i, PQfnumber(res, "prezzo")));
            const char* nome = PQgetvalue(res, i, PQfnumber(res, "nome"));

            // Assegna gli attributi all'i-esimo Prodotto in forniti
            forniti[i].ID = ID;
            forniti[i].descrizione = descrizione;
            forniti[i].prezzo = prezzo;
            forniti[i].nome = nome;
            }
            risultato.first = rows; // Ritorna il numero di righe dei prodottiDisponibili
            risultato.second = forniti; // Ritorna l'array di prodotti disponibili
        } else {     // Se non ci sono oggetti
            risultato.first = 0;
            risultato.second = nullptr;
        }
        PQclear(res);
        return risultato;
    }
    catch(...) // Controlla che la query sia andata a buon fine
    {
        risultato.first = -1;
        risultato.second = nullptr;
        PQclear(res);
        return risultato; 
    }
}
