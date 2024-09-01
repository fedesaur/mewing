#include "ricercaProdotti.h"
bool cercaProdottiDisponibili(int clientSocket)
{
    int USER_ID;
    PGresult *res;
    redisContext *c2r; // c2r contiene le info sul contesto
	redisReply *reply; // reply contiene le risposte da Redis

	c2r = redisConnect(REDIS_IP, REDIS_PORT); // Effettua la connessione a Redis
	Con2DB db(HOSTNAME, DB_PORT, USERNAME, PASSWORD, DB_NAME); // Effettua la connessione al database

    reply = RedisCommand(c2r, "XREVRANGE %s + - COUNT 1", READ_STREAM);
    if (reply == nullptr || reply->type != REDIS_REPLY_ARRAY || reply->elements == 0)
    {
        std::cerr << "Errore nel comando Redis o stream vuoto" << std::endl;
    }

    std::string id = reply->element[0]->element[1]->element[1]->str; 
    USER_ID = atoi(id.c_str()); // ID Customer

    // Recupera le informazioni su tutti i prodotti disponibili
    std::pair<int, Prodotto*> risultato1 = recuperaCarrello(USER_ID, db, res, clientSocket);
    if (risultato1.first == -1) return false; // Se vi sono errori
    int righeCar = risultato1.first;
    Prodotto* carrello = risultato1.second;
    std::pair<int, Prodotto*> risultato2 = recuperaProdottiDisponibili(db, res, clientSocket);
    if (risultato2.first == -1) return false; // Se vi sono errori
    //...recuperati i prodotti, permette operazioni con quelli trovati e quelli anche nel carrello  
    int righe = risultato2.first;
    Prodotto* prodottiDisponibili = risultato2.second;
    //for (int i = 0; i < righe; i++) std::cout << prodottiDisponibili[i].ID << prodottiDisponibili[i].descrizione << prodottiDisponibili[i].prezzo << prodottiDisponibili[i].nome <<  prodottiDisponibili[i].fornitore << std::endl;
    
    // Libera lo spazio occupato dai prodotti nel carrello e/o quelli disponibili in vendita
    delete[] risultato1.second;
    delete[] risultato2.second;
    return true;
}

std::pair<int, Prodotto*>  recuperaProdottiDisponibili(Con2DB db, PGresult *res, int clientSocket)
{
    std::pair <int, Prodotto*> risultato;
    int rows;
    char comando[1000];
    sprintf(comando, "SELECT pr.id, pr.descrizione, pr.nome, pr.prezzo, fr.nome AS nomeF FROM prodotto pr, fornitore fr WHERE pr.fornitore = fr.id");
    res = db.ExecSQLtuples(comando);
    if (PQresultStatus(res) != PGRES_TUPLES_OK)
    {
        risultato.first = -1;
        risultato.second = nullptr;
        return risultato; // Controlla che la query sia andata a buon fine
    }
    rows = PQntuples(res);
    if (rows > 0)
    {
        // Prima mostriamo all'utente i prodotti disponibili..
        std::string request = "PRODOTTI DISPONIBILI:\n"; //... e lo stampa
	    send(clientSocket, request.c_str(), request.length(), 0); // Invia il messaggio pre-impostato all'utente
        Prodotto* prodottiDisponibili = new Prodotto[rows];

        for (int i = 0; i < rows; i++)
        {
            // Recupera gli attributi dei prodotti dalla query sopra svolta...
            int ID = atoi(PQgetvalue(res, i, PQfnumber(res, "id")));
            const char* descrizione = PQgetvalue(res, i, PQfnumber(res, "descrizione"));
            double prezzo = atof(PQgetvalue(res, i, PQfnumber(res, "prezzo")));
            const char* nome = PQgetvalue(res, i, PQfnumber(res, "nome"));
            const char* fornitore = PQgetvalue(res, i, PQfnumber(res, "nomeF"));
            // ...e li invia all'utente così che possa visualizzarli ed effettuarci operazioni
            std::string prodotto = std::to_string(i+1) + ") ID Prodotto: " + std::to_string(ID) + 
            " Nome Prodotto: " + nome + 
            " Descrizione: " + descrizione + 
            " Fornitore: " + fornitore + 
            " Prezzo Prodotto: " + std::to_string(prezzo) + "\n";
	        send(clientSocket, prodotto.c_str(), prodotto.length(), 0);

            // Assegna gli attributi all'i-esimo Prodotto in prodottiDisponibili
            prodottiDisponibili[i].ID = ID;
            prodottiDisponibili[i].descrizione = descrizione;
            prodottiDisponibili[i].prezzo = prezzo;
            prodottiDisponibili[i].nome = nome;
            prodottiDisponibili[i].fornitore = fornitore;
        }
        risultato.first = rows; // Ritorna il numero di righe dei prodottiDisponibili
        risultato.second = prodottiDisponibili; // Ritorna l'array di prodotti disponibili
        PQclear(res);
        return risultato;
    }
    // Se non ci sono oggetti
	std::string request = "Nessun prodotto disponibile!\n"; // Seleziona la frase del turno
	send(clientSocket, request.c_str(), request.length(), 0); // Invia il messaggio pre-impostato all'utente
    risultato.first = 0;
    risultato.second = nullptr;
    return risultato;
}
