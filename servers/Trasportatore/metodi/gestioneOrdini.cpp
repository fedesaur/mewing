#include "gestioneOrdini.h"

bool gestioneOrdini(int clientSocket)
{
    int TRASPORTER_ID;
    int RIGHE;
    char buffer[1024] = {0};
    int OPERAZIONI_DISPONIBILI = 4;
    std::string OPERAZIONI[] = {"1) Recupera dettagli ordine\n", "2) Ricerca nuovo ordine\n", "3) Conferma consegna di un ordine", "Altrimenti digita Q per terminare\n"};
    std::tuple<int, Corriere*, Ordine*> risultato;
    Corriere* CORRIERI;
    Ordine* ORDINI;
    redisContext *c2r; // c2r contiene le info sul contesto
	redisReply *reply; // reply contiene le risposte da Redis

	c2r = redisConnect(REDIS_IP, REDIS_PORT); // Effettua la connessione a Redis
    reply = RedisCommand(c2r, "XREVRANGE %s + - COUNT 1", READ_STREAM);
    if (reply == nullptr || reply->type != REDIS_REPLY_ARRAY || reply->elements == 0)
    {
        std::cerr << "Errore nel comando Redis o stream vuoto" << std::endl;
    }

    std::string id = reply->element[0]->element[1]->element[1]->str; 
    TRASPORTER_ID = stoi(id); // ID Trasportatore
    // Usa una funzione ausiliaria per recuperare gli ordini in corso di consegna
    risultato = ordiniCorrenti(clientSocket);
    if (std::get<0>(risultato) == -1) return false;  // C'è stato un errore nella query
    
    bool terminaConnessione = false;
    while(!terminaConnessione)
    {
        RIGHE = std::get<0>(risultato);
        CORRIERI = std::get<1>(risultato);
        ORDINI = std::get<2>(risultato);
        
        // Mostra i corrieri e i loro ordini
        mostraCorrenti(clientSocket,RIGHE,CORRIERI,ORDINI);
        std::string request = "\nQuale operazione vuoi svolgere?\n";
	    send(clientSocket, request.c_str(), request.length(), 0); // Invia il messaggio pre-impostato all'utente
        
        // Mostra le operazioni disponibili
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
                } else if (isdigit(messaggio[0])) { //isNumber è una funzione ausiliaria in lib
                    int opzione = std::stoi(messaggio) - 1;
                    bool esito = false;
                    switch (opzione)
                    {
                        case 0:
                            if (RIGHE > 0)
                            {
                                std::string request = "Di quale ordine vuoi recuperare i dettagli?(Digita il numero)\n";
	                            send(clientSocket, request.c_str(), request.length(), 0); 
                                int indice = riceviIndice(clientSocket, RIGHE);
                            } else {
                                std::string errore = "Non ci sono ordini per cui recuperare i dettagli!\n";
                                send(clientSocket, errore.c_str(), errore.length(), 0);
                            }
                            attendiInput = false;
                            break;
                        case 1:
                            std::cout << "Funzione Ricerca Ordini non implementata\n";
                            send(clientSocket, "Funzione non ancora implementata.\n", 35, 0);
                            attendiInput = false;
                            break;
                        case 2:
                            if (RIGHE > 0)
                            {
                                std::string request = "Di quale ordine vuoi confermare la consegna?(Digita il numero)\n";
	                            send(clientSocket, request.c_str(), request.length(), 0); 
                                int indice = riceviIndice(clientSocket, RIGHE);
                                esito = consegnaOrdine(clientSocket, ORDINI[indice].ID);
                            } else {
                                std::string errore = "Non ci sono ordini per cui confermare la consegna!\n";
                                send(clientSocket, errore.c_str(), errore.length(), 0);
                            }
                            attendiInput = false;
                            break;
                        default:
                            std::string errore = "Opzione non valida, riprova.\n";
                            send(clientSocket, errore.c_str(), errore.length(), 0);
                            break;                               
                    }
                    if (esito)
                    {
                        delete[] ORDINI;
                        delete[] CORRIERI;
                        risultato = ordiniCorrenti(clientSocket);
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
    // Libera la memoria occupata da Ordini e Corrieri
    delete[] ORDINI;
    delete[] CORRIERI;
    return true;
}

bool consegnaOrdine(int clientSocket, int ordineID)
{
    char comando[1000];
    PGresult *res;
    Con2DB db(HOSTNAME, DB_PORT, USERNAME_TRAS, PASSWORD_TRAS, DB_NAME); // Effettua la connessione al database
    sprintf(comando, "INSERT INTO ordineconse(id, datacons) VALUES (%d, NOW())", ordineID);
    try
    {
        res = db.ExecSQLcmd(comando);
        std::string request = "Consegna effettuata!";
	    send(clientSocket, request.c_str(), request.length(), 0);
        PQclear(res);
        return true; 
    }
    catch(...)
    {
        std::string errore = "C'è stato un errore nel database\n";
		send(clientSocket, errore.c_str(), errore.length(), 0); // Invia il messaggio pre-impostato all'utente
        PQclear(res);
        return false;
    }
    
}