#include "ricercaProdotti.h"

bool ricercaProdotti(int clientSocket)
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

    // Recupera i prodotti nel carrelo tramite una funzione ausiliaria
    std::pair<int, Prodotto*> risultato1 = recuperaCarrello(USER_ID, db, res, clientSocket);
    if (risultato1.first == -1) return false; // Se vi sono errori
    
    std::pair<int, Prodotto*> risultato2 = recuperaProdottiDisponibili(db, res, clientSocket);
    if (risultato2.first == -1) return false; // Se vi sono errori
    else if (risultato2.first == 0) return true; // Se non ci sono prodotti disponibili, non ci sono operazioni da svolgere
    
    //...recuperati i prodotti, permette operazioni con quelli trovati e quelli anche nel carrello
    bool continuaOperazione = true;
    while (continuaOperazione) {continuaOperazione = aggiungiAlCarrello(risultato1, risultato2, clientSocket);}
    
    // Libera lo spazio occupato dai prodotti nel carrello e/o quelli disponibili in vendita
    delete[] risultato1.second;
    delete[] risultato2.second;
    return true;
}

std::pair<int, Prodotto*> recuperaProdottiDisponibili(Con2DB db, PGresult *res, int clientSocket)
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

bool aggiungiAlCarrello(Con2DB db, PGresult *res, int USER_ID, std::pair<int, Prodotto*> carrello, std::pair<int, Prodotto*> disponibili, int clientSocket)
{
    char buffer[1024] = {0};
    int RIGHE_CARRELLO = carrello.first;
    Prodotto* CARRELLO = carrello.second;
    int RIGHE_DISPONIBILI = disponibili.first;
    Prodotto* PRODOTTI = disponibili.second;
    
    if (RIGHE_DISPONIBILI > 0) // Se ci sono prodotti NON nel carrello da aggiungere...
    {
        for (int i = 0; i < RIGHE_DISPONIBILI; i++) // Mostra all'utente i prodotti disponibili
        {
            std::string prodotto = std::to_string(i+1) + ") ID Prodotto: " + std::to_string(PRODOTTI[i].ID) + 
                " Nome Prodotto: " + PRODOTTI[i].nome + 
                " Descrizione: " + PRODOTTI[i].descrizione + 
                " Fornitore: " + PRODOTTI[i].fornitore + 
                " Prezzo Prodotto: " + std::to_string(PRODOTTI[i].fornitore) + "\n";
	        send(clientSocket, prodotto.c_str(), prodotto.length(), 0);
        }
        //...incomincia le operazioni per aggiungerli
        bool attendiInput = true;
        // Mostra all'utente gli elementi nel carrello tramite una funzione ausiliaria in recuperaCarrello.h
        mostraCarrello(clientSocket, CARRELLO, RIGHE_CARRELLO, res);
        std::string request = "\nQuale prodotto vuoi aggiungere? (Digita il numero)\nOppure digita Q per terminare la connessione\n";
	    send(clientSocket, request.c_str(), request.length(), 0); // Invia il messaggio pre-impostato all'utente
        
        while (attendiInput) // Richiede all'utente un input valido
        {
            int bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
            if (bytesRead > 0) 
            {
                std::string messaggio(buffer, bytesRead);
                messaggio.erase(std::remove(messaggio.begin(), messaggio.end(), '\n'), messaggio.end()); // Rimuove eventuali newline

                if (messaggio == "q" || messaggio == "Q") attendiInput = false;
                else if (isNumber(messaggio))
                {
                    int indice = stoi(messaggio) - 1;
                    if (indice >= 0 && indice < RIGHE_DISPONIBILI)
                    {
                        attendiInput = false;
                        int idP = PRODOTTI[indice].ID;
                        int quantita = richiediQuantita(clientSocket);
                        bool esito = aggiungiProdottoDB(idP, USER_ID, quantita, db, res);
                        if (esito)
                        {
                            aggiungiProdotto(carrello, disponibili, indice, quantita);
                            std::string successo = "Prodotto aggiunto al carrello con successo!\n";
                            send(clientSocket, successo.c_str(), successo.length(), 0);
                        } else {
                            std::string errore = "C'è stato un errore nella query\n";
                            send(clientSocket, errore.c_str(), errore.length(), 0);
                        } 
                    } else {
                        std::string errore = "Opzione non valida, riprova.\n";
                        send(clientSocket, errore.c_str(), errore.length(), 0);
                    }
                } else {
                    std::string errore = "Input non valido, riprova.\n";
                    send(clientSocket, errore.c_str(), errore.length(), 0);
                }
            }
        }   
        return true;
    }
    std::string errore = "Non ci sono prodotti disponibili!\n"; // Seleziona la frase del turno
	send(clientSocket, errore.c_str(), errore.length(), 0); // Invia il messaggio pre-impostato all'utente
    return false;
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
            if isNumber(messaggio)
            {
                int numero = stoi(messaggio);
                if (numero > 0) quantita = numero;
                else
                {
                    std::string errore = "Quantità non valida\n";
	                send(clientSocket, request.c_str(), request.length(), 0); // Invia il messaggio pre-impostato all'utente
                }
            } else {
                std::string errore = "Input non valido\n";
	            send(clientSocket, request.c_str(), request.length(), 0); // Invia il messaggio pre-impostato all'utente
            }
        } else {
            std::string errore = "Input non valido\n";
	        send(clientSocket, request.c_str(), request.length(), 0); // Invia il messaggio pre-impostato all'utente
        }
    }
    return quantita;
}

bool aggiungiProdottoDB(int idProdotto, int userID, int quantita, Con2DB db, PGresult *res)
{
    char comando[1000];

    sprintf(comando, "INSERT INTO prodincart(carrello, prodotto, quantita) VALUES (%d, %d, %d)", userID, idProdotto, quantita);
    try
    {
        res = db.ExecSQLcmd(comando);
        PQclear(res);
        return true;
    }
    catch(...)
    {
        // Se ci sono errori nella query, vengono catturati da catch
        return false;
    }
    return true;
}

void aggiungiProdotto(std::pair<int, Prodotto*> carrello, std::pair<int, Prodotto*> disponibili, int indice, int quanto)
{
    int RIGHE_CARRELLO = carrello.first;
    Prodotto* CARRELLO = carrello.second;
    int RIGHE_DISPONIBILI = disponibili.first;
    Prodotto* PRODOTTI = disponibili.second;

    if (RIGHE_CARRELLO == 0 && carrello.second == nullptr) {
        carrello.first = 1;
        RIGHE_CARRELLO = 1;
        carrello.second = new Prodotto[1];
    }
    else {
        Prodotto* newCart = new Prodotto[RIGHE_CARRELLO+1];
        for (int i = 0; i < RIGHE_CARRELLO; i++) newCart[i] = CARRELLO[i]; // Sposta i prodotti nel nuovo carrello
        delete[] carrello.second;
        carrello.first++;
        RIGHE_CARRELLO++;
        carrello.second = newCart;
    }
    CARRELLO[RIGHE_CARRELLO] = PRODOTTI[indice];
    CARRELLO[RIGHE_CARRELLO].quantita = quanto;
    rimuoviProdotto(PRODOTTI[indice].ID, PRODOTTI, RIGHE_DISPONIBILI);
    disponibili.first--;
    return;
}