#include "gestisciCorrieri.h"

bool gestisciCorrieri(int clientSocket)
{
    int TRASPORTER_ID;
    int RIGHE;
    char buffer[1024] = {0};
    char comando[1000];
    int OPERAZIONI_DISPONIBILI = 3;
    std::string OPERAZIONI[] = {"1) Aggiungi un corriere\n", "2) Rimuovi un corriere\n", "Altrimenti digita Q per terminare\n"};
    Corriere* CORRIERI;
    PGresult *res;
    redisContext *c2r; // c2r contiene le info sul contesto
	redisReply *reply; // reply contiene le risposte da Redis

	c2r = redisConnect(REDIS_IP, REDIS_PORT); // Effettua la connessione a Redis
	Con2DB db(HOSTNAME, DB_PORT, USERNAMEC, PASSWORDC, DB_NAME); // Effettua la connessione al database

    reply = RedisCommand(c2r, "XREVRANGE %s + - COUNT 1", READ_STREAM);
    if (reply == nullptr || reply->type != REDIS_REPLY_ARRAY || reply->elements == 0)
    {
        std::cerr << "Errore nel comando Redis o stream vuoto" << std::endl;
    }

    std::string id = reply->element[0]->element[1]->element[1]->str; 
    TRASPORTER_ID = stoi(id); // ID Trasportatore
    // Usa una funzione ausiliaria per recuperare il carrello dell'utente
    std::pair <int, Corriere*> risultato = recuperaCorrieri(clientSocket);
    if (risultato.first == -1) return false;  // C'è stato un errore nella query
    
    RIGHE = risultato.first;
    CORRIERI = risultato.second;

    bool terminaConnessione = false;
    while(!terminaConnessione)
    {
        // Mostra all'utente gli elementi nel carrello tramite una funzione ausiliaria
        mostraCorrieri(clientSocket, RIGHE, risultato.second);
        std::string request = "\nQuale operazione vuoi svolgere?\n";
	    send(clientSocket, request.c_str(), request.length(), 0); // Invia il messaggio pre-impostato all'utente
        for (int i = 0; i < OPERAZIONI_DISPONIBILI; i++) send(clientSocket, OPERAZIONI[i].c_str(), OPERAZIONI[i].length(), 0);
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
                    attendiInput = false;
                    terminaConnessione = true;
                } else if (isNumber(messaggio)) { //isNumber è una funzione ausiliaria in lib
                    int opzione = std::stoi(messaggio) - 1;
                    bool esito = false;
                    switch (opzione)
                    {
                        case 0:
                            esito = registraCorriere(clientSocket);
                            if (esito) 
                            {
                                    delete[] risultato.second;
                                    risultato = recuperaCorrieri(clientSocket);
                                    RIGHE = risultato.first;
                                    CORRIERI = risultato.second;
                            }
                            attendiInput = false;
                            break;
                        case 1:
                        {
                            std::string request = "Quale corriere vuoi rimuovere?\n";
	                        send(clientSocket, request.c_str(), request.length(), 0); 
                            int indice = riceviIndice(clientSocket, RIGHE);
                            int idC = CORRIERI[indice].ID;
                            esito = rimuoviCorriere(clientSocket, idC);
                            if (esito) 
                            {
                                    delete[] risultato.second;
                                    risultato = recuperaCorrieri(clientSocket);
                                    RIGHE = risultato.first;
                                    CORRIERI = risultato.second;
                            }
                            attendiInput = false;
                            break;}

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
        }   
    }
    delete[] risultato.second; // Libera la memoria occupata dal carrello
    return true;
}
