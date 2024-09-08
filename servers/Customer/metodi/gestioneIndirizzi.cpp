#include "gestioneIndirizzi.h"

bool gestisciIndirizzi(int clientSocket)
{
    int CUSTOMER_ID;
    int RIGHE;
    char buffer[1024] = {0};
    char comando[1000];
    int OPERAZIONI_DISPONIBILI = 4;
    std::pair <int, Indirizzo*> risultato;
    std::string OPERAZIONI[] = {"1) Aggiungi Indirizzo\n", "2) Modifica Indirizzo\n", "3) Rimuovi Indirizzo\n", "Altrimenti digita Q per terminare\n"};
    Indirizzo* INDIRIZZI;
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
    CUSTOMER_ID = std::stoi(id); // ID Customer
    
    // Usa una funzione ausiliaria per recuperare gli indirizzi registrati
    risultato = recuperaIndirizzi(clientSocket);
    if (risultato.first == -1) return false;  // C'è stato un errore nella query
    
    RIGHE = risultato.first;
    INDIRIZZI = risultato.second;
    bool terminaConnessione = false;
    
    while(!terminaConnessione)
    {
        // Mostra all'utente gli elementi nel carrello tramite una funzione ausiliaria
        mostraIndirizzi(clientSocket, RIGHE, INDIRIZZI);
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
                            {
                            // Permette ad un Customer di aggiungere un Indirizzo
                            esito = aggiungiIndirizzo(clientSocket);
                            attendiInput = false;
                            break;
                            }
                        case 1:
                            // Ancora da implementare la modifica degli indirizzi
                            attendiInput = false;
                            break;
                        case 2:
                        {
                            std::string request = "\nQuale indirizzo vuoi rimuovere?\n";
	                        send(clientSocket, request.c_str(), request.length(), 0); // Invia il messaggio pre-impostato all'utente
                            int index = riceviIndice(clientSocket, RIGHE);
                            try
                            {
                                int idInd = INDIRIZZI[index].ID;
                                sprintf(comando, "DELETE FROM indirizzo WHERE id = %d", idInd);
                                res = db.ExecSQLcmd(comando);
                                sprintf(comando, "DELETE FROM custadd WHERE customer = %d AND addr = %d", CUSTOMER_ID, idInd);
                                res = db.ExecSQLcmd(comando);
                                std::string request = "Indirizzo rimosso con successo!\n\n";
	                            send(clientSocket, request.c_str(), request.length(), 0); // Invia il messaggio pre-impostato all'utente
                                esito = true;
                                attendiInput = false;
                            }
                            catch(...)
                            {
                                std::string errore = "C'è stato un errore nel database\n\n";
		                        send(clientSocket, errore.c_str(), errore.length(), 0); // Invia il messaggio pre-impostato all'utente   
                            }
                            break;
                        }
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
                risultato = recuperaIndirizzi(clientSocket);
                RIGHE = risultato.first;
                INDIRIZZI = risultato.second;
            }
        }   
    }
    delete[] risultato.second; // Libera la memoria occupata dal carrello
    return true;
}

int riceviIndice(int clientSocket, int righe)
{
    char buffer[1024] = {0};
    int indice = -1;
    while (indice == -1)
    {
        int bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
        if (bytesRead > 0) 
        {
            std::string messaggio(buffer, bytesRead);
            messaggio.erase(std::remove(messaggio.begin(), messaggio.end(), '\n'), messaggio.end()); // Rimuove eventuali newline
            if (isNumber(messaggio)) //isNumber è una funzione ausiliaria in lib
            {
                int numero = std::stoi(messaggio) - 1;
                if (numero > 0 && numero < righe) indice = numero;
                else 
                {
                    std::string errore = "Input non valido\n";
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
    return indice;
}