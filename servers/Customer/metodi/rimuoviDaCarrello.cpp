#include "rimuoviDaCarrello.h"
bool rimuoviProdotti(int clientSocket)
{
    int USER_ID;
    int RIGHE;
    char comando[1000];
    Prodotto* CARRELLO;
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
    USER_ID = stoi(id); // ID Customer

    // Usa una funzione ausiliaria per recuperare il carrello dell'utente
    std::pair <int, Prodotto*> risultato = recuperaCarrello(USER_ID, db, res, clientSocket);
    if (risultato.first == -1) return false; // C'è stato un errore nella query

    RIGHE = risultato.first;
    CARRELLO = risultato.second;
    if (rows > 0)
    {
        // Mostriamo all'utente i prodotti nel suo carrello
        double totale = atof(PQgetvalue(res, 0, PQfnumber(res, "totale"))); //Recupera il totale...
        std::string request = "\nPRODOTTI NEL CARRELLO (TOTALE :" + std::to_string(totale) + "):\n"; //... e lo stampa
	    send(clientSocket, request.c_str(), request.length(), 0); // Invia il messaggio pre-impostato all'utente
        for (int i = 0; i < RIGHE; i++)
        {
            // Recupera gli attributi dei prodotti dal carrello...
            int ID = CARRELLO[i].ID;
            const char* descrizione = CARRELLO[i].descrizione;
            double prezzo = CARRELLO[i].prezzo;
            const char* nomeP = CARRELLO[i].nome;
            const char* fornitore = CARRELLO[i].fornitore;
            int quantita = CARRELLO[i].quantita;
            // ...e li invia all'utente così che possa visualizzarli ed effettuarci operazioni
            std::string prodotto = std::to_string(i+1) + ") ID Prodotto: " + std::to_string(ID) +
             " Nome Prodotto: " + nomeP + 
             " Descrizione: " + descrizione + 
             " Fornitore: " + fornitore + 
             " Prezzo Prodotto: " + std::to_string(prezzo) + 
             " Quantità :" + std::to_string(quantita) + "\n";
	        send(clientSocket, prodotto.c_str(), prodotto.length(), 0);
        }
        // Per ora l'operazione finisce qui
        return true;
    }
    // Se non ci sono oggetti
	std::string request = "\nNessun prodotto nel carrello!\n"; // Seleziona la frase del turno
	send(clientSocket, request.c_str(), request.length(), 0); // Invia il messaggio pre-impostato all'utente
    delete[] risultato.second; // Libera la memoria occupata dal carrello
    return true;
}