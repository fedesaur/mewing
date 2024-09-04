#include "modificaFornito.h"

bool modificaFornito(int clientSocket)
{
    int PRODUCER_ID;
    int RIGHE;
    char buffer[1024] = {0};
    Prodotto* FORNITI;
    redisContext *c2r; // c2r contiene le info sul contesto
	redisReply *reply; // reply contiene le risposte da Redis

	c2r = redisConnect(REDIS_IP, REDIS_PORT); // Effettua la connessione a Redis

    reply = RedisCommand(c2r, "XREVRANGE %s + - COUNT 1", READ_STREAM);
    if (reply == nullptr || reply->type != REDIS_REPLY_ARRAY || reply->elements == 0)
    {
        std::cerr << "Errore nel comando Redis o stream vuoto" << std::endl;
    }

    std::string id = reply->element[0]->element[1]->element[1]->str; 
    PRODUCER_ID = stoi(id); // ID Customer
    // Usa una funzione ausiliaria per recuperare il carrello dell'utente
    std::pair <int, Prodotto*> risultato = recuperaForniti();
    if (risultato.first == -1) return false;  // C'è stato un errore nella query

    if (risultato.first > 0)
    {
        bool terminaConnessione = false;
        while(!terminaConnessione)
        {
            RIGHE = risultato.first;
            FORNITI = risultato.second;
            // Mostra all'utente gli elementi nel carrello tramite una funzione ausiliaria
            mostraForniti(clientSocket, FORNITI, RIGHE);
            std::string request = "Quale prodotto vuoi modificare? (Digita il numero)\nOppure digita Q per terminare la connessione\n";
	        send(clientSocket, request.c_str(), request.length(), 0); // Invia il messaggio pre-impostato all'utente
            bool attendiInput = true;
            while (attendiInput)
            {
                int bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
                if (bytesRead > 0) 
                {
                    std::string messaggio(buffer, bytesRead);
                    messaggio.erase(std::remove(messaggio.begin(), messaggio.end(), '\n'), messaggio.end()); // Rimuove eventuali newline

                    if (messaggio == "q" || messaggio == "Q") 
                    {
                        delete[] risultato.second;
                        attendiInput = false;
                        terminaConnessione = true;
                    } else if (isNumber(messaggio)){ //isNumber è una funzione ausiliaria in lib
                        int indice = stoi(messaggio) - 1;
                        if (indice >= 0 && indice < RIGHE)
                        {
                            attendiInput = false;
                            bool esito = modificaAttributoFornito(FORNITI[indice], PRODUCER_ID, clientSocket); 
                            // Se la rimozione dal DB va bene, viene eseguita anche quella locale
                            if (esito)
                            {
                                std::string successo = "Prodotto modificato con successo!\n";
                                send(clientSocket, successo.c_str(), successo.length(), 0);
                                delete[] risultato.second; // Libera la memoria occupata dai prodotti
                                risultato = recuperaForniti();
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
        }
        delete[] risultato.second; // Libera la memoria occupata dal carrello
        return true;
    }
    // Se non ci sono oggetti
	std::string request = "\nNessun prodotto fornito!\n"; // Seleziona la frase del turno
    send(clientSocket, request.c_str(), request.length(), 0); // Invia il messaggio pre-impostato all'utente
    delete[] risultato.second; // Libera la memoria occupata dal carrello
    return true;
}

bool modificaAttributoFornito(Prodotto* prodotto, int clientSocket)
{
    char buffer[1024] = {0};
    int NUMERO_OPZIONI = 4;
    std::string OPZIONI[] = {"1)Cambia nome del prodotto\n", "2)Cambia descrizione del prodotto\n", "3)Cambia prezzo del prodotto\n", "Oppure digita Q per terminare la connessione\n"};
    bool continuaConnessione = true;
    while (continuaConnessione)
    {
        std::string = "Nome attuale: " + prodotto->nome + " Descrizione attuale: " + prodotto->descrizione + " Prezzo del prodotto: " + std::to_string(prodotto->prezzo) + "\n";
        for (int i = 0; i < NUMERO_OPZIONI ; i++) send(clientSocket, OPZIONI[i].c_str(), OPZIONI[i].length(), 0); // Stampa le opzioni
        bool attendiInput = true;
        while (attendiInput)
        {
            int bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
            if (bytesRead > 0)
            {
                std::string messaggio(buffer, bytesRead);
                messaggio.erase(std::remove(messaggio.begin(), messaggio.end(), '\n'), messaggio.end()); // Rimuove eventuali newline

                if (messaggio == "q" || messaggio == "Q") {
                    attendiInput = false;
                    continuaConnessione = false; // Termina la connessione
                } else if (std::isdigit(messaggio[0])) {
                    int opzione = std::stoi(messaggio) - 1;
                    std::pair<std::string,bool> risultato;
                    switch (opzione)
                    {
                        case 0:
                            risultato = cambiaNomeProdotto(clientSocket, prodotto->ID)
                            if (risultato.second)
                            {
                                std::string conferma = "Nome cambiato!\n";
                                send(clientSocket, conferma.c_str(), conferma.length(), 0);
                                prodotto->nome = risultato.first.c_str();
                            }
                            break;
                        case 1:
                            risultato = cambiaDescrizioneProdotto(clientSocket, prodotto->ID);
                            if (risultato.second)
                            {   
                                std::string conferma = "Descrizione cambiata!\n";
                                send(clientSocket, conferma.c_str(), conferma.length(), 0);
                                prodotto->descrizione = risultato.first.c_str();
                            }        
                            break;
                        case 2:
                            risultato = cambiaPrezzoProdotto(clientSocket, prodotto->ID);
                            if (risultato.second)
                            {   
                                std::string conferma = "Prezzo cambiato!\n";
                                send(clientSocket, conferma.c_str(), conferma.length(), 0);
                                prodotto->prezzo = stod(risultato.first);
                            }        
                            break;
                        default: // Se gli altri casi non sono stati accettati...
                        std::string errore = "Opzione non valida, riprova.\n";
                        send(clientSocket, errore.c_str(), errore.length(), 0);
                        break;
                    }
                } else {
                std::string errore = "Input non valido, riprova.\n";
                send(clientSocket, errore.c_str(), errore.length(), 0);
                }
            }
        }
    }
    return true;
}

std::pair<std::string,bool> cambiaNomeProdotto(int clientSocket, int idProdotto)
{
    PGresult *res;
    Con2DB db(HOSTNAME, DB_PORT, USERNAME, PASSWORD, DB_NAME); // Effettua la connessione al database
    char comando[1000];
    char buffer[1024] = {0};
    std::pair<std::string,bool> risultato;
    std::string request = "Inserisci il nuovo nome del prodotto\n"; 
	int bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
    if (bytesRead > 0)
    {
        // Chiede all'utente il nuovo nome
        std::string messaggio(buffer, bytesRead);
        messaggio.erase(std::remove(messaggio.begin(), messaggio.end(), '\n'), messaggio.end()); // Rimuove eventuali newline
        if (messaggio.length() > 100)
        {                    
            std::string errore = "Il nome deve avere massimo 100 caratteri\n";
            send(clientSocket, errore.c_str(), errore.length(), 0);
            risultato.first = "";
            risultato.second = false;
            return risultato;
        }
        sprintf(comando, "UPDATE prodotto SET nome = '%s' WHERE id = %d", messaggio.c_str(), idProdotto);
        try
        {
            res = db.ExecSQLcmd(comando);
            risultato.first = messaggio;
            risultato.second = true;
            PQclear(res);
        }
        catch(const std::exception& e)
        {
            risultato.first = "";
            risultato.second = false;
        }
        return risultato;
    }
    std::string errore = "Input non valido\n";
    send(clientSocket, errore.c_str(), errore.length(), 0);
    risultato.first = "";
    risultato.second = false;
    return risultato;
}

std::pair<std::string,bool> cambiaDescrizioneProdotto(int clientSocket, int idProdotto)
{
    PGresult *res;
    Con2DB db(HOSTNAME, DB_PORT, USERNAME, PASSWORD, DB_NAME); // Effettua la connessione al database
    char comando[1000];
    char buffer[1024] = {0};
    std::pair<std::string,bool> risultato;
    std::string request = "Inserisci la nuova descrizione del prodotto\n"; 
	int bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
    if (bytesRead > 0)
    {
        // Chiede all'utente il nuovo nome
        std::string messaggio(buffer, bytesRead);
        messaggio.erase(std::remove(messaggio.begin(), messaggio.end(), '\n'), messaggio.end()); // Rimuove eventuali newline
        sprintf(comando, "UPDATE prodotto SET descrizione = '%s' WHERE id = %d", messaggio.c_str(), idProdotto);
        try
        {
            res = db.ExecSQLcmd(comando);
            risultato.first = messaggio;
            risultato.second = true;
            PQclear(res);
        }
        catch(const std::exception& e)
        {
            risultato.first = "";
            risultato.second = false;
        }
        return risultato;
    }
    std::string errore = "Input non valido\n";
    send(clientSocket, errore.c_str(), errore.length(), 0);
    risultato.first = "";
    risultato.second = false;
    return risultato;
}

std::pair<std::string,bool> cambiaPrezzoProdotto(int clientSocket, int idProdotto)
{
    PGresult *res;
    Con2DB db(HOSTNAME, DB_PORT, USERNAME, PASSWORD, DB_NAME); // Effettua la connessione al database
    char comando[1000];
    char buffer[1024] = {0};
    std::pair<std::string,bool> risultato;
    std::string request = "Inserisci il nuovo prezzo del prodotto\n"; 
	int bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
    if (bytesRead > 0)
    {
        double newPrezzo = atof(messaggio);
        sprintf(comando, "UPDATE prodotto SET prezzo = %f WHERE id = %d", newPrezzo, idProdotto);
        try
        {
            res = db.ExecSQLcmd(comando);
            risultato.first = std::to_string(newPrezzo);
            risultato.second = true;
            PQclear(res);
        }
        catch(const std::exception& e)
        {
            risultato.first = "";
            risultato.second = false;
        }
        return risultato;
    }
    std::string errore = "Input non valido\n";
    send(clientSocket, errore.c_str(), errore.length(), 0);
    risultato.first = "";
    risultato.second = false;
    return risultato;
}
