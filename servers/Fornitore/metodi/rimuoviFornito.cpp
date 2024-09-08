#include "rimuoviFornito.h"

bool rimuoviFornito(int clientSocket)
{
    int PRODUCER_ID;
    int RIGHE;
    char buffer[1024] = {0};
    Prodotto* FORNITI;
    PGresult *res;
    redisContext *c2r;
    redisReply *reply;

    c2r = redisConnect(REDIS_IP, REDIS_PORT);
    Con2DB db(HOSTNAME, DB_PORT, USERNAMEP, PASSWORDP, DB_NAME);

    reply = RedisCommand(c2r, "XREVRANGE %s + - COUNT 1", READ_STREAM);
    if (reply == nullptr || reply->type != REDIS_REPLY_ARRAY || reply->elements == 0)
    {
        std::cerr << "Errore nel comando Redis o stream vuoto" << std::endl;
        return false;
    }

    std::string id = reply->element[0]->element[1]->element[1]->str; 
    PRODUCER_ID = stoi(id);
    
    std::pair <int, Prodotto*> risultato = recuperaForniti();
    if (risultato.first == -1) return false;  // C'è stato un errore nella query
    
    RIGHE = risultato.first;
    FORNITI = risultato.second;

    if (RIGHE > 0)
    {
        bool terminaConnessione = false;
        while(!terminaConnessione && RIGHE > 0)
        {
            mostraForniti(clientSocket, FORNITI, RIGHE);
            std::string request = "Quale prodotto vuoi rimuovere? (Digita il numero)\nOppure digita Q per terminare la connessione\n";
	        send(clientSocket, request.c_str(), request.length(), 0);
            
            bool attendiInput = true;
            while (attendiInput)
            {
                int bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
                if (bytesRead > 0) 
                {
                    std::string messaggio(buffer, bytesRead);
                    messaggio.erase(std::remove(messaggio.begin(), messaggio.end(), '\n'), messaggio.end());

                    if (messaggio == "q" || messaggio == "Q") 
                    {
                        delete[] risultato.second;
                        attendiInput = false;
                        terminaConnessione = true;
                    } else if (isNumber(messaggio)){
                        int indice = stoi(messaggio) - 1;
                        if (indice >= 0 && indice < RIGHE)
                        {
                            attendiInput = false;
                            int idP = FORNITI[indice].ID;
                            bool esito = rimuoviDaFornitiDB(idP, PRODUCER_ID, db, res);
                            
                            if (esito)
                            {
                                rimuoviProdotto(idP, FORNITI, RIGHE);
                                RIGHE--;
                                std::string successo = "Prodotto rimosso con successo!\n";
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
        }
        delete[] risultato.second;
        return true;
    }

    std::string request = "\nNessun prodotto fornito!\n";
    send(clientSocket, request.c_str(), request.length(), 0);
    delete[] risultato.second;
    return true;
}


bool rimuoviDaFornitiDB(int idProdotto, int producerID, Con2DB db, PGresult *res)
{
    char comando[1000];
    sprintf(comando, "DELETE FROM prodotto WHERE id = %d AND fornitore = %d", idProdotto, producerID);
    try
    {
        res = db.ExecSQLcmd(comando);
        PQclear(res);
        return true;
    }
    catch(...)
    {
        PQclear(res);
        // Se ci sono errori nella query, vengono catturati da catch
        return false;
    }
    return true;
}
