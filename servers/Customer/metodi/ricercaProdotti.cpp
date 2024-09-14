#include "ricercaProdotti.h"

bool ricercaProdotti(int clientSocket)
{
    char buffer[1024] = {0};
    char comando[1000];
    int USER_ID;
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
    // Seleziona tutti i prodotti disponibili che NON sono nel carrello

    try
    {
        sprintf(comando, "SELECT pr.id, pr.descrizione, pr.nome, pr.prezzo, fr.nome AS nomeF "
        "FROM prodotto pr, fornitore fr WHERE pr.fornitore = fr.id");
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
            PQclear(res);
            bool terminaConnessione = false;
            while (!terminaConnessione)
            {

                for (int i = 0; i < RIGHE; i++) // Mostra i prodotti disponibili all'utente
                {
                    std::string prodotto = std::to_string(i+1) + ") ID Prodotto: " + std::to_string(prodottiDisponibili[i].ID) + 
                    " Nome Prodotto: " + prodottiDisponibili[i].nome + 
                    " Descrizione: " + prodottiDisponibili[i].descrizione + 
                    " Fornitore: " + prodottiDisponibili[i].fornitore + 
                    " Prezzo Prodotto: " + std::to_string(prodottiDisponibili[i].prezzo) + "\n";
	                send(clientSocket, prodotto.c_str(), prodotto.length(), 0);
                }
                std::string request = "\nQuale prodotto vuoi aggiungere? (Digita il numero)\nOppure digita Q per terminare la connessione\n";
	            send(clientSocket, request.c_str(), request.length(), 0); // Invia il messaggio pre-impostato all'utente
                int bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
                if (bytesRead > 0) 
                {
                    std::string messaggio(buffer, bytesRead);
                    messaggio.erase(std::remove(messaggio.begin(), messaggio.end(), '\n'), messaggio.end()); // Rimuove eventuali newline

                    if (messaggio == "q" || messaggio == "Q") terminaConnessione = false;
                    else if (isNumber(messaggio))
                    {
                        int indice = stoi(messaggio) - 1;
                        if (indice >= 0 && indice < RIGHE)
                        {
                            int idP = prodottiDisponibili[indice].ID;
                            int quantita = richiediQuantita(clientSocket); // Chiede la quantita di prodotto desiderata dall'utente
                            bool esito = aggiungiCarrelloDB(idP, USER_ID, quantita, db, res); // Aggiunge il prodotto al carrello
                            if (esito)
                            {
                                std::string successo = "Prodotto aggiunto al carrello con successo!\n\n";
                                send(clientSocket, successo.c_str(), successo.length(), 0);
                            } else {
                                std::string errore = "C'è stato un errore nella query\n\n";
                                send(clientSocket, errore.c_str(), errore.length(), 0);
                            } 
                        } else {
                            std::string errore = "Opzione non valida, riprova.\n\n";
                            send(clientSocket, errore.c_str(), errore.length(), 0);
                        }
                    } else {
                        std::string errore = "Input non valido, riprova.\n\n";
                        send(clientSocket, errore.c_str(), errore.length(), 0);
                    }
                }
                std::string errore = "Input non valido, riprova.\n\n";
                send(clientSocket, errore.c_str(), errore.length(), 0);
            }
            PQclear(res);
            return true;
        }   
        // Se non ci sono oggetti
        PQclear(res);
	    std::string request = "Nessun prodotto disponibile!\n\n"; // Seleziona la frase del turno
	    send(clientSocket, request.c_str(), request.length(), 0); // Invia il messaggio pre-impostato all'utente
        return false;
    }
    catch(...)
    {
        std::string errore = "C'è stato un errore nel database!\n"; // Seleziona la frase del turno
	    send(clientSocket, errore.c_str(), errore.length(), 0); // Invia il messaggio pre-impostato all'utente
        return false; // Controlla che la query sia andata a buon fine
    }
}

int richiediQuantita(int clientSocket)
{
    int quantita = -1;
    char buffer[1024] = {0};
    std::string request = "In che quantita ne vuoi?\n";
	send(clientSocket, request.c_str(), request.length(), 0); // Invia il messaggio pre-impostato all'utente
    while (quantita == -1)
    {
        int bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
        if (bytesRead > 0)
        {
            std::string messaggio(buffer, bytesRead);
            messaggio.erase(std::remove(messaggio.begin(), messaggio.end(), '\n'), messaggio.end()); // Rimuove eventuali newline
            if (isNumber(messaggio)) 
            {
                int numero = stoi(messaggio);
                if (numero > 0) {
                    quantita = numero;
                } else {
                    std::string errore = "Quantità non valida\n";
	                send(clientSocket, errore.c_str(), errore.length(), 0); // Invia il messaggio pre-impostato all'utente
                }
            } else {
                std::string errore = "Input non valido\n";
	            send(clientSocket, errore.c_str(), errore.length(), 0); // Invia il messaggio pre-impostato all'utente
            }
        } else {
            std::string errore = "Input non valido, riprova.\n";
            send(clientSocket, errore.c_str(), errore.length(), 0);
        }
    }
  return quantita;
}

bool aggiungiCarrelloDB(int idProdotto, int userID, int quantita, Con2DB db, PGresult *res)
{
    char comando[1000];
    int rows;
    sprintf(comando, "SELECT quantita FROM prodincart WHERE prodotto = %d AND carrello = %d", idProdotto, userID);
    try
    {
        res = db.ExecSQLtuples(comando);
        rows = PQntuples(res);
        if (rows == 1) // Il prodotto già c'è, perciò aumenta il numero di prodotti presenti
        {
            int plus = atoi(PQgetvalue(res, 0, PQfnumber(res, "quantita"))) + quantita;
            sprintf(comando, "UPDATE prodincart SET quantita = %d WHERE prodotto = %d AND carrello = %d", plus, idProdotto, userID);
        }
        else {sprintf(comando, "INSERT INTO prodincart(carrello, prodotto, quantita) VALUES (%d, %d, %d)", userID, idProdotto, quantita);}
        res = db.ExecSQLcmd(comando);
        PQclear(res);
        return true;
    }
    catch(...)
    {
        // Se ci sono errori nella query, vengono catturati da catch
        return false;
    }
}
