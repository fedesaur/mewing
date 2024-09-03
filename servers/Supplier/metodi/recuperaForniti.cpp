#include "recuperaForniti.h"

std::pair<int, Prodotto*> recuperaForniti(int ID, Con2DB db, PGresult *res)
{
    std::pair <int, Prodotto*> risultato;
    int rows;
    char comando[1000];
    // Recupera i prodotti forniti
    sprintf(comando, "SELECT id, descrizione, prezzo, nome FROM prodotto WHERE fornitore = %d", ID);
    res = db.ExecSQLtuples(comando);
    if (PQresultStatus(res) != PGRES_TUPLES_OK) // Controlla che la query sia andata a buon fine
    {
        risultato.first = -1;
        risultato.second = nullptr;
        return risultato; 
    }
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

void mostraCarrello(int clientSocket, Prodotto* forniti, int righe)
{
    if (righe > 0)
    {
        std::string request = "\nPRODOTTI NEL CARRELLO:\n"; //... e lo stampa
	    send(clientSocket, request.c_str(), request.length(), 0); // Invia il messaggio pre-impostato all'utente
        for (int i = 0; i < righe; i++)
        {
            // Recupera gli attributi dei prodotti dal carrello...
            int ID = forniti[i].ID;
            const char* descrizione = forniti[i].descrizione;
            double prezzo = forniti[i].prezzo;
            const char* nomeP = forniti[i].nome;
            // ...e li invia all'utente così che possa visualizzarli ed effettuarci operazioni
            std::string prodotto = std::to_string(i+1) + ") ID Prodotto: " + std::to_string(ID) +
             " Nome Prodotto: " + nomeP + 
             " Descrizione: " + descrizione +  
             " Prezzo Prodotto: " + std::to_string(prezzo) + "\n";
	        send(clientSocket, prodotto.c_str(), prodotto.length(), 0);
        }
    } else {
        std::string request = "Non ci sono prodotti forniti!\n"; //... e lo stampa
	    send(clientSocket, request.c_str(), request.length(), 0); // Invia il messaggio pre-impostato all'utente
    }
    return;
}