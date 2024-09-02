#include "rimuoviDaCarrello.h"
bool rimuoviProdotti(int clientSocket)
{
    int USER_ID;
    int RIGHE;
    char buffer[1024] = {0};
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
    if (risultato.first == -1) return false;  // C'è stato un errore nella query
    RIGHE = risultato.first;
    CARRELLO = risultato.second;
    bool terminaConnessione = false;
    if (RIGHE > 0)
    {
        do
        {
            // Mostra all'utente gli elementi nel carrello tramite una funzione ausiliaria
            mostraCarrello(clientSocket, CARRELLO, RIGHE, res);
            std::string request = "Quale prodotto vuoi rimuovere? (Digita il numero)\nOppure digita Q per terminare la connessione\n";
	        send(clientSocket, request.c_str(), request.length(), 0); // Invia il messaggio pre-impostato all'utente
            bool attendiInput = true;
            do
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
                            int idP = CARRELLO[indice].ID;
                            bool esito = rimuoviDaCarrelloDB(idP, USER_ID, db, res); 
                            // Se la rimozione dal DB va bene, viene eseguita anche quella locale
                            if (esito)
                            {
                                rimuoviProdotto(idP, CARRELLO, RIGHE); // rimuoviProdotto è una funzione ausiliaria in lib
                                RIGHE--;
                                std::string successo = "Prodotto rimosso con successo!\n";
                                send(clientSocket, successo.c_str(), successo.length(), 0);
                            } else{
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
            } while (attendiInput);
                
        }while (!terminaConnessione && RIGHE > 0);
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
