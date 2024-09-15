#include "gestisciCarrello.h"

bool gestisciCarrello(int clientSocket)
{
    int CUSTOMER_ID;
    int RIGHE;
    char buffer[1024] = {0};
    char comando[1000];
    int OPERAZIONI_DISPONIBILI = 5;
    std::pair <int, Prodotto*> risultato;
    std::string OPERAZIONI[] = {"1) Aggiungi prodotto (normale)\n", "2) Aggiungi prodotto (per nome)\n", "3) Rimuovi prodotto dal carrello \n", "4) Ordina prodotti nel carrello \n" , "Altrimenti digita Q per terminare\n"};
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
    CUSTOMER_ID = stoi(id); // ID Trasportatore
    // Usa una funzione ausiliaria per recuperare il carrello dell'utente
    risultato = recuperaCarrello(clientSocket);
    if (risultato.first == -1) return false;  // C'è stato un errore nella query
    
    RIGHE = risultato.first;
    CARRELLO = risultato.second;
    bool terminaConnessione = false;
    
    while(!terminaConnessione)
    {
        // Mostra all'utente gli elementi nel carrello tramite una funzione ausiliaria
        mostraCarrello(clientSocket, CARRELLO, RIGHE);
        std::string request = "\nQuale operazione vuoi svolgere?\n";
	    send(clientSocket, request.c_str(), request.length(), 0); // Invia il messaggio pre-impostato all'utente
        for (int i = 0; i < OPERAZIONI_DISPONIBILI; i++) send(clientSocket, OPERAZIONI[i].c_str(), OPERAZIONI[i].length(), 0);
        bool attendiInput = true;
        while (attendiInput)
        {
            bool esito = false;
            int bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
            if (bytesRead > 0) 
            {
                std::string messaggio(buffer, bytesRead);
                messaggio.erase(std::remove(messaggio.begin(), messaggio.end(), '\n'), messaggio.end()); // Rimuove eventuali newline
                if (messaggio == "q" || messaggio == "Q") 
                {
                    attendiInput = false;
                    terminaConnessione = true;
                } else if (isdigit(messaggio[0])){
                    int opzione = std::stoi(messaggio) - 1;
                    bool esito = false;
                    switch (opzione)
                    {
                        case 0:
                            // Permette ad un Customer di cercare un prodotto e aggiungerlo al carrello
                            esito = ricercaProdotti(clientSocket);
                            attendiInput = false;
                            break;
                        case 1:
                            // Ancora da implementare la ricerca dei prodotti
                            attendiInput = false;
                            break;
                        case 2:
                            if (RIGHE > 0)
                            {
                                std::string request = "Quale prodotto vuoi rimuovere? (Digita il numero)\n";
	                            send(clientSocket, request.c_str(), request.length(), 0); // Invia il messaggio pre-impostato all'utente
                                int indice = riceviIndice(clientSocket, RIGHE);
                                sprintf(comando, "DELETE FROM prodincart WHERE prodotto = %d AND carrello = %d", CARRELLO[indice].ID, CUSTOMER_ID);
                                try
                                {
                                    res = db.ExecSQLcmd(comando);
                                    PQclear(res);
                                    std::string successo = "Prodotto rimosso correttamente dal carrello!\n";
	                                send(clientSocket, successo.c_str(), successo.length(), 0); // Invia il messaggio pre-impostato all'utente
                                    esito = true;
                                }
                                catch(...)
                                {
                                    std::string errore = "C'è stato un errore nel database!\n";
	                                send(clientSocket, errore.c_str(), errore.length(), 0); // Invia il messaggio pre-impostato all'utente
                                    // Se ci sono errori nella query, vengono catturati da catch
                                }
                            } else {
                                std::string request = "\nNessun prodotto nel carrello da rimuovere!\n";
                                send(clientSocket, request.c_str(), request.length(), 0); // Invia il messaggio pre-impostato all'utente
                            }
                            attendiInput = false;
                            break;
                        case 3:
                            esito = effettuaOrdine(clientSocket, CUSTOMER_ID);
                            attendiInput = false;
                            break;

                        default:
                            std::string errore = "Opzione non valida, riprova.\n";
                            send(clientSocket, errore.c_str(), errore.length(), 0);
                            break;                               
                    }
                } else {
                    std::string errore = "Input non valido, riprova.\n";
                    send(clientSocket, errore.c_str(), errore.length(), 0);
                }
            } else {
                std::string errore = "Input non valido, riprova.\n";
                send(clientSocket, errore.c_str(), errore.length(), 0);
            }
            if (esito)
            {
                delete[] risultato.second;
                risultato = recuperaCarrello(clientSocket);
                RIGHE = risultato.first;
                CARRELLO = risultato.second;
            }
        }   
    }
    delete[] risultato.second; // Libera la memoria occupata dal carrello
    return true;
}

bool effettuaOrdine(int clientSocket, int customerID)
{
    return false;
}
