#include "recuperaProdotti.h"

std::pair<int, Prodotto*> recuperaProdotti(int clientSocket)
{
    char buffer[1024] = {0};
    char comando[1000];
    std::pair<int, Prodotto*> risultato;
    PGresult *res;
    Con2DB db(HOSTNAME, DB_PORT, USERNAME_CUST, PASSWORD_CUST, DB_NAME); // Effettua la connessione al database
    try
    {
        sprintf(comando, "SELECT pr.id, pr.descrizione, pr.nome, fr.nome AS nomeF, pr.prezzo FROM prodotto pr, fornitore fr WHERE pr.fornitore = fr.id");
        res = db.ExecSQLtuples(comando);
        int RIGHE = PQntuples(res);
        if (RIGHE > 0)
        {
            // Prima mostriamo all'utente i prodotti disponibili..
            std::string request = "PRODOTTI DISPONIBILI:\n"; //... e lo stampa
	    send(clientSocket, request.c_str(), request.length(), 0); // Invia il messaggio pre-impostato all'utente
            Prodotto* prodottiDisponibili = new Prodotto[RIGHE];

            for (int i = 0; i < RIGHE; i++)
            {
            // Recupera gli attributi dei prodotti dalla query sopra svolta...
                int ID = atoi(PQgetvalue(res, i, PQfnumber(res, "id")));
                const char* descrizione = PQgetvalue(res, i, PQfnumber(res, "descrizione"));
                double prezzo = atof(PQgetvalue(res, i, PQfnumber(res, "prezzo")));
                const char* nome = PQgetvalue(res, i, PQfnumber(res, "nome"));
                const char* fornitore = PQgetvalue(res, i, PQfnumber(res, "nomeF"));

            // Assegna gli attributi all'i-esimo Prodotto in prodottiDisponibili
                prodottiDisponibili[i].ID = ID;
                prodottiDisponibili[i].descrizione = descrizione;
                prodottiDisponibili[i].prezzo = prezzo;
                prodottiDisponibili[i].nome = nome;
                prodottiDisponibili[i].fornitore = fornitore;
            }
            risultato.first = RIGHE;
            risultato.second = prodottiDisponibili;
        } else {
            risultato.first = 0;
            risultato.second =  nullptr;
        }
    }
    catch(...) // Controlla che la query sia andata a buon fine
    {
        std::string errore = "C'è stato un errore nel database!\n"; // Seleziona la frase del turno
	    send(clientSocket, errore.c_str(), errore.length(), 0); // Invia il messaggio pre-impostato all'utente
        risultato.first = -1;
        risultato.second = nullptr;
        
    }
    PQclear(res);
    return risultato; 
}

std::pair<int, Prodotto*> recuperaProdottiPerNome(int clientSocket, std::string nome)
{
    char buffer[1024] = {0};
    char comando[1000];
    std::pair<int, Prodotto*> risultato;
    PGresult *res;
    Con2DB db(HOSTNAME, DB_PORT, USERNAME_CUST, PASSWORD_CUST, DB_NAME);  // Effettua la connessione al database
    try
    {
        std::string searched = "%" + nome + "%";
        sprintf(comando, "SELECT pr.id, pr.descrizione, pr.nome, pr.prezzo, fr.nome AS nomeF "
        "FROM prodotto pr, fornitore fr WHERE pr.fornitore = fr.id AND pr.nome LIKE '%s' ", searched.c_str());
        res = db.ExecSQLtuples(comando);
        int RIGHE = PQntuples(res);
        if (RIGHE > 0)
        {
            // Prima mostriamo all'utente i prodotti disponibili..
            std::string request = "PRODOTTI DISPONIBILI:\n"; //... e lo stampa
	        send(clientSocket, request.c_str(), request.length(), 0); // Invia il messaggio pre-impostato all'utente
            Prodotto* prodottiDisponibili = new Prodotto[RIGHE];

            for (int i = 0; i < RIGHE; i++)
            {
            // Recupera gli attributi dei prodotti dalla query sopra svolta...
                int ID = atoi(PQgetvalue(res, i, PQfnumber(res, "id")));
                const char* descrizione = PQgetvalue(res, i, PQfnumber(res, "descrizione"));
                double prezzo = atof(PQgetvalue(res, i, PQfnumber(res, "prezzo")));
                const char* nome = PQgetvalue(res, i, PQfnumber(res, "nome"));
                const char* fornitore = PQgetvalue(res, i, PQfnumber(res, "nomeF"));

            // Assegna gli attributi all'i-esimo Prodotto in prodottiDisponibili
                prodottiDisponibili[i].ID = ID;
                prodottiDisponibili[i].descrizione = descrizione;
                prodottiDisponibili[i].prezzo = prezzo;
                prodottiDisponibili[i].nome = nome;
                prodottiDisponibili[i].fornitore = fornitore;
            }
            risultato.first = RIGHE;
            risultato.second = prodottiDisponibili;
        } else {
            risultato.first = 0;
            risultato.second =  nullptr;
        }
    }
    catch(...) // Controlla che la query sia andata a buon fine
    {
        std::string errore = "C'è stato un errore nel database!\n"; // Seleziona la frase del turno
	    send(clientSocket, errore.c_str(), errore.length(), 0); // Invia il messaggio pre-impostato all'utente
        risultato.first = -1;
        risultato.second = nullptr;
    }
    PQclear(res);
    return risultato; 
}

 void mostraProdotti(int clientSocket, Prodotto* prodotti, int righe) {
        if (righe > 0) {
            std::string request = "\nPRODOTTI DISPONIBILI:\n";
            send(clientSocket, request.c_str(), request.length(), 0);
            for (int i = 0; i < righe; i++) {
                std::string prodotto = std::to_string(i+1) + ") ID Prodotto: " + std::to_string(prodotti[i].ID) + 
                                       " Nome Prodotto: " + prodotti[i].nome + 
                                       " Descrizione: " + prodotti[i].descrizione + 
                                       " Fornitore: " + prodotti[i].fornitore + 
                                       " Prezzo Prodotto: " + std::to_string(prodotti[i].prezzo) + "\n";
                send(clientSocket, prodotto.c_str(), prodotto.length(), 0);
            }
        } else {
            std::string request = "Non ci sono prodotti disponibili!\n";
            send(clientSocket, request.c_str(), request.length(), 0);
        }
    }
