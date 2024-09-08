#include "rimuoviDaCarrello.h"
bool rimuoviDaCarrello(int clientSocket)
{
    int USER_ID;
    int RIGHE;
    char buffer[1024] = {0};
    char comando[1000];
    std::pair <int, Prodotto*> risultato;
    Prodotto* CARRELLO;
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
    USER_ID = stoi(id); // ID Customer
    // Usa una funzione ausiliaria per recuperare il carrello dell'utente
    risultato = recuperaCarrello(clientSocket);
    if (risultato.first == -1) return false;  // C'è stato un errore nella query
    RIGHE = risultato.first;
    CARRELLO = risultato.second;
    bool terminaConnessione = false;
    if (RIGHE > 0)
    {
        while (!terminaConnessione && RIGHE > 0)
        {
            // Mostra all'utente gli elementi nel carrello tramite una funzione ausiliaria
            mostraCarrello(clientSocket, CARRELLO, RIGHE);
            std::string request = "Quale prodotto vuoi rimuovere? (Digita il numero)\nOppure digita Q per terminare la connessione\n";
	        send(clientSocket, request.c_str(), request.length(), 0); // Invia il messaggio pre-impostato all'utente
            bool attendiInput = true;
            bool rimozione = false;
            while (attendiInput)
            {
                int bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
                if (bytesRead > 0) 
                {
                    std::string messaggio(buffer, bytesRead);
                    messaggio.erase(std::remove(messaggio.begin(), messaggio.end(), '\n'), messaggio.end()); // Rimuove eventuali newline

                    if (messaggio == "q" || messaggio == "Q")
                    {
                        terminaConnessione = true;
                        attendiInput = false;
                    } 
                    else if (isNumber(messaggio))
                    {
                        int indice = stoi(messaggio) - 1;
                        if (indice >= 0 && indice < RIGHE)
                        {
                            int idP = CARRELLO[indice].ID;
                            bool rimozione = rimuoviDaCarrelloDB(idP, USER_ID, db, res);
                            if (rimozione)
                            {
                                std::string successo = "Prodotto aggiunto al carrello con successo!\n\n";
                                send(clientSocket, successo.c_str(), successo.length(), 0);
                                attendiInput = false;
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
            if (rimozione)
            {
                delete[] risultato.second; // Libera la memoria occupata dal carrello
                risultato = recuperaCarrello(clientSocket);
                if (risultato.first == -1) return false;  // C'è stato un errore nella query
                RIGHE = risultato.first;
                CARRELLO = risultato.second;
            } 
        }
        delete[] risultato.second; // Libera la memoria occupata dal carrello
        return true;
    }
    // Se non ci sono oggetti
	std::string request = "\nNessun prodotto nel carrello!\n"; // Seleziona la frase del turno
    send(clientSocket, request.c_str(), request.length(), 0); // Invia il messaggio pre-impostato all'utente
    delete[] risultato.second; // Libera la memoria occupata dal carrello
    return true;
}

bool rimuoviDaCarrelloDB(int idProdotto, int userID, Con2DB db, PGresult *res)
{
    char comando[1000];
    sprintf(comando, "DELETE FROM prodincart WHERE prodotto = %d AND carrello = %d", idProdotto, userID);
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
