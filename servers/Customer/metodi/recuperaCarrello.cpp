#include "recuperaCarrello.h"

std::pair<int, Prodotto*> recuperaCarrello(int clientSocket)
{
    std::pair <int, Prodotto*> risultato;
    int rows;
    int USER_ID;
    char comando[1000];
    PGresult *res;
    redisContext *c2r; // c2r contiene le info sul contesto
	redisReply *reply; // reply contiene le risposte da Redis

	c2r = redisConnect(REDIS_IP, REDIS_PORT); // Effettua la connessione a Redis
	Con2DB db(HOSTNAME, DB_PORT, USERNAME_CUST, PASSWORD_CUST, DB_NAME); // Effettua la connessione al database

    reply = RedisCommand(c2r, "XREVRANGE %s + - COUNT 1", READ_STREAM);
    if (reply == nullptr || reply->type != REDIS_REPLY_ARRAY || reply->elements == 0)
    {
        std::cerr << "Errore nel comando Redis o stream vuoto" << std::endl;
    }

    std::string id = reply->element[0]->element[1]->element[1]->str; 
    USER_ID = atoi(id.c_str()); // ID Customer
    
    sprintf(comando, "SELECT pr.id, pr.descrizione, pr.nome, pr.prezzo, fr.nome AS nomeF, cr.totale, pc.quantita "
    "FROM prodotto pr, carrello cr, prodincart pc, fornitore fr WHERE pc.prodotto = pr.id "
    "AND pc.carrello = cr.customer AND pr.fornitore = fr.id AND cr.customer = %d", USER_ID);
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

        // Mostriamo all'utente i prodotti nel suo carrello
        Prodotto* carrello = new Prodotto[rows];
        // Recupera i prodotti e li memorizza in carrello
        for (int i = 0; i < rows; i++)
        {
            // Recupera gli attributi dei prodotti dalla query sopra svolta...
            int ID = atoi(PQgetvalue(res, i, PQfnumber(res, "id")));
            const char* descrizione = PQgetvalue(res, i, PQfnumber(res, "descrizione"));
            double prezzo = atof(PQgetvalue(res, i, PQfnumber(res, "prezzo")));
            const char* nome = PQgetvalue(res, i, PQfnumber(res, "nome"));
            const char* fornitore = PQgetvalue(res, i, PQfnumber(res, "nomeF"));
            int quantita = atoi(PQgetvalue(res, i, PQfnumber(res, "quantita")));

            // Assegna gli attributi all'i-esimo Prodotto in prodottiDisponibili
            carrello[i].ID = ID;
            carrello[i].descrizione = descrizione;
            carrello[i].prezzo = prezzo;
            carrello[i].nome = nome;
            carrello[i].fornitore = fornitore;
            carrello[i].quantita = quantita;
        }
        risultato.first = rows; // Ritorna il numero di righe dei prodottiDisponibili
        risultato.second = carrello; // Ritorna l'array di prodotti disponibili
    } else {     // Se non ci sono oggetti
        risultato.first = 0;
        risultato.second = nullptr;
    }
    PQclear(res);
    return risultato;
}

void mostraCarrello(int clientSocket, Prodotto* carrello, int righe)
{
    if (righe > 0)
    {
        std::string request = "\nPRODOTTI NEL CARRELLO:\n"; //... e lo stampa
	    send(clientSocket, request.c_str(), request.length(), 0); // Invia il messaggio pre-impostato all'utente
        for (int i = 0; i < righe; i++)
        {
            // Recupera gli attributi dei prodotti dal carrello...
            int ID = carrello[i].ID;
            const char* descrizione = carrello[i].descrizione;
            double prezzo = carrello[i].prezzo;
            const char* nomeP = carrello[i].nome;
            const char* fornitore = carrello[i].fornitore;
            int quantita = carrello[i].quantita;
            // ...e li invia all'utente così che possa visualizzarli ed effettuarci operazioni
            std::string prodotto = std::to_string(i+1) + ") ID Prodotto: " + std::to_string(ID) +
             " Nome Prodotto: " + nomeP + 
             " Descrizione: " + descrizione + 
             " Fornitore: " + fornitore + 
             " Prezzo Prodotto: " + std::to_string(prezzo) + 
             " Quantità :" + std::to_string(quantita) + "\n";
	        send(clientSocket, prodotto.c_str(), prodotto.length(), 0);
        }
    } else {
        std::string request = "Non ci sono prodotti nel carrello!\n"; //... e lo stampa
	    send(clientSocket, request.c_str(), request.length(), 0); // Invia il messaggio pre-impostato all'utente
    }
    return;
}
