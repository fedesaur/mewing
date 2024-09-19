#include "recuperaCarrello.h"

std::pair<int, Prodotto*> getCarrellos(int clientSocket){
    std::pair <int, Prodotto*> risultato;
    int rows;
    char comando[1000];
    PGresult *res = nullptr;
    Con2DB db(HOSTNAME, DB_PORT, USERNAME_CUST, PASSWORD_CUST, DB_NAME);
    
    sprintf(comando, "SELECT pr.id, pr.descrizione, pr.nome, pr.prezzo, fr.nome AS nomeF, cr.totale, pc.quantita "
                     "FROM prodotto pr, carrello cr, prodincart pc, fornitore fr WHERE pc.prodotto = pr.id "
                     "AND pc.carrello = cr.customer AND pr.fornitore = fr.id AND cr.customer = %d", 1);
                     
    try {
        // Esegui la query SQL
        res = db.ExecSQLtuples(comando);
        rows = PQntuples(res);
        


        if (rows > 0) {
            Prodotto* carrello = new Prodotto[rows];

            // Recupera i prodotti e li memorizza in carrello
	        for (int i = 0; i < rows; i++) {
	    	// Recupera gli attributi dei prodotti dalla query SQL
	    	int ID = atoi(PQgetvalue(res, i, PQfnumber(res, "id")));
	    	const char* descrizione = PQgetvalue(res, i, PQfnumber(res, "descrizione"));
	    	double prezzo = atof(PQgetvalue(res, i, PQfnumber(res, "prezzo")));
	    	const char* nome = PQgetvalue(res, i, PQfnumber(res, "nome"));
	    	const char* fornitore = PQgetvalue(res, i, PQfnumber(res, "nomeF"));
	    	int quantita = atoi(PQgetvalue(res, i, PQfnumber(res, "quantita")));

	    	// Assegna direttamente i valori alle stringhe (conversione automatica da const char* a std::string)
	    	carrello[i].ID = ID;
	    	carrello[i].descrizione = descrizione;   // std::string converte automaticamente da const char*
	    	carrello[i].prezzo = prezzo;
	    	carrello[i].nome = nome;
	    	carrello[i].fornitore = fornitore;
	    	carrello[i].quantita = quantita;
		}

            risultato.first = rows; // Numero di prodotti
            risultato.second = carrello; // Array di prodotti

        } else { // Nessun prodotto nel carrello
            risultato.first = 0;
            risultato.second = nullptr;
        }

    } catch (...) {
        // Gestione dell'errore nel database
        std::string errore = "C'è stato un errore nel database!\n";
        send(clientSocket, errore.c_str(), errore.length(), 0);
        risultato.first = -1;
        risultato.second = nullptr;
    }

    // Liberazione della memoria per res
    if (res != nullptr) {
        PQclear(res);
    }

    return risultato;
    
}

std::pair<int, Prodotto*> recuperaCarrello(int clientSocket)
{
    std::pair <int, Prodotto*> risultato;
    int rows;
    int USER_ID;
    char comando[1000];
    PGresult *res = nullptr;
    redisContext *c2r; // c2r contiene le info sul contesto
    redisReply *reply; // reply contiene le risposte da Redis

    // Connessione a Redis
    c2r = redisConnect(REDIS_IP, REDIS_PORT);
    if (c2r == nullptr || c2r->err) {
        std::cerr << "Errore: impossibile connettersi a Redis: " << c2r->errstr << std::endl;
        risultato.first = -1;
        risultato.second = nullptr;
        return risultato;
    }

    // Connessione al database
    Con2DB db(HOSTNAME, DB_PORT, USERNAME_CUST, PASSWORD_CUST, DB_NAME);

    // Esegui il comando Redis per ottenere lo USER_ID
    /*
    reply = (redisReply*)redisCommand(c2r, "XREVRANGE %s + - COUNT 1", READ_STREAM);
    if (reply == nullptr || reply->type != REDIS_REPLY_ARRAY || reply->elements == 0) {
        std::cerr << "Errore nel comando Redis o stream vuoto" << std::endl;
        if (reply) freeReplyObject(reply);  // Libera la risposta Redis, se allocata
        redisFree(c2r);
        risultato.first = -1;
        risultato.second = nullptr;
        return risultato;
    }

    if (reply->element[0] == nullptr || reply->element[0]->elements < 2 || reply->element[0]->element[1]->str == nullptr) {
        std::cerr << "Errore: struttura della risposta Redis non valida" << std::endl;
        freeReplyObject(reply);
        redisFree(c2r);
        risultato.first = -1;
        risultato.second = nullptr;
        return risultato;
    }

    // Ottieni lo USER_ID dall'elemento Redis
    std::string id = reply->element[0]->element[1]->str;
    */
    std::cout << "quiqui" << std::endl;
    USER_ID = 1; // Converti USER_ID

    // Libera la memoria della risposta Redis
    //freeReplyObject(reply);
    //redisFree(c2r);

    // Costruisci la query SQL per recuperare il carrello
    sprintf(comando, "SELECT pr.id, pr.descrizione, pr.nome, pr.prezzo, fr.nome AS nomeF, cr.totale, pc.quantita "
                     "FROM prodotto pr, carrello cr, prodincart pc, fornitore fr WHERE pc.prodotto = pr.id "
                     "AND pc.carrello = cr.customer AND pr.fornitore = fr.id AND cr.customer = %d", USER_ID);

    try {
        // Esegui la query SQL
        res = db.ExecSQLtuples(comando);
        rows = PQntuples(res);
        
        std::cout << rows << std::endl;

        if (rows > 0) {
            Prodotto* carrello = new Prodotto[rows];

            // Recupera i prodotti e li memorizza in carrello
	        for (int i = 0; i < rows; i++) {
	    	// Recupera gli attributi dei prodotti dalla query SQL
	    	int ID = atoi(PQgetvalue(res, i, PQfnumber(res, "id")));
	    	const char* descrizione = PQgetvalue(res, i, PQfnumber(res, "descrizione"));
	    	double prezzo = atof(PQgetvalue(res, i, PQfnumber(res, "prezzo")));
	    	const char* nome = PQgetvalue(res, i, PQfnumber(res, "nome"));
	    	const char* fornitore = PQgetvalue(res, i, PQfnumber(res, "nomeF"));
	    	int quantita = atoi(PQgetvalue(res, i, PQfnumber(res, "quantita")));

	    	// Assegna direttamente i valori alle stringhe (conversione automatica da const char* a std::string)
	    	carrello[i].ID = ID;
	    	carrello[i].descrizione = descrizione;   // std::string converte automaticamente da const char*
	    	carrello[i].prezzo = prezzo;
	    	carrello[i].nome = nome;
	    	carrello[i].fornitore = fornitore;
	    	carrello[i].quantita = quantita;
	    	
	    	std::cout << carrello[i].descrizione << std::endl;
		}

            risultato.first = rows; // Numero di prodotti
            risultato.second = carrello; // Array di prodotti

        } else { // Nessun prodotto nel carrello
            risultato.first = 0;
            risultato.second = nullptr;
        }

    } catch (...) {
        // Gestione dell'errore nel database
        std::string errore = "C'è stato un errore nel database!\n";
        send(clientSocket, errore.c_str(), errore.length(), 0);
        risultato.first = -1;
        risultato.second = nullptr;
    }

    // Liberazione della memoria per res
    if (res != nullptr) {
        PQclear(res);
    }

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
