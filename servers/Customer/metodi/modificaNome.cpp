#include "modificaNome.h"

bool modificaNome(int clientSocket)
{
    int USER_ID;
    PGresult *res;
    redisContext *c2r; // c2r contiene le info sul contesto
	redisReply *reply; // reply contiene le risposte da Redis
    char comando[1000];
    char buffer[1024] = {0};
    std::string OPZIONI[] = {"1) Cambia Nome\n", "2) Cambia Cognome\n", "Altrimenti digita Q per terminare\n"};

	c2r = redisConnect(REDIS_IP, REDIS_PORT); // Effettua la connessione a Redis
	Con2DB db(HOSTNAME, DB_PORT, USERNAME, PASSWORD, DB_NAME); // Effettua la connessione al database

    reply = RedisCommand(c2r, "XREVRANGE %s + - COUNT 1", READ_STREAM);
    if (reply == nullptr || reply->type != REDIS_REPLY_ARRAY || reply->elements == 0)
    {
        std::cerr << "Errore nel comando Redis o stream vuoto" << std::endl;
        return false;
    }
    
    ReadStreamMsgVal(reply, 0, 1, 1, id); 
    USER_ID = atoi(id);
    sprintf(comando, "SELECT nome, cognome FROM customers WHERE id = %d", USER_ID);
    db.ExecSQLtuples(comando);
    if (PQresultStatus(res) != PGRES_TUPLES_OK) return false;
    // Recupera nome e cognome dell'utente dal database
    std::string nome = PQgetvalue(res, 0, PQfnumber(res, "nome"));
    std::string cognome = PQgetvalue(res, 0, PQfnumber(res, "cognome"));
    bool attendiInput = true;
    bool concludiOperazione = false;
    do
    {
        // Mostra nome e cognome all'utente
        std::string request = "Nome attuale: " + nome + " Cognome Attuale: " + cognome + "\n"; 
	    send(clientSocket, request.c_str(), request.length(), 0);
        //Stampa all'utente le opzioni disponibili
        for (int i = 0; i < OPZIONI.length(); i++) send(clientSocket, OPZIONI[i].c_str(), OPZIONI[i].length(), 0);
        int bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
        if (bytesRead > 0)
        {
            std::string messaggio(buffer, bytesRead);
            messaggio.erase(std::remove(messaggio.begin(), messaggio.end(), '\n'), messaggio.end()); // Rimuove eventuali newline

            if (messaggio == "q" || messaggio == "Q") {
                concludiOperazione = true; // Termina la connessione
            } else if (std::stoi(messaggio[0]) == 1) {
                std::pair<std::string,bool> risultato = cambiaNome(db, res, clientSocket);
                if (risultato.second) 
                {
                    std::string conferma = "Nome cambiato!\n";
                    send(clientSocket, conferma.c_str(), conferma.length(), 0);
                    nome = risultato.first;
                }
            } else if (std::stoi(messaggio[0]) == 2) {
                std::pair<std::string,bool> risultato = cambiaCognome(db, res, clientSocket);
                if (risultato.second)
                {
                    std::string conferma = "Cognome cambiato!\n";
                    send(clientSocket, conferma.c_str(), conferma.length(), 0);
                    cognome = risultato.first;
                }               
            } else {
                std::string errore = "Opzione non valida, riprova.\n";
                send(clientSocket, errore.c_str(), errore.length(), 0);
            }
        } else {
            std::string errore = "Input non valido, riprova.\n";
            send(clientSocket, errore.c_str(), errore.length(), 0);
        }
    } while (attendiInput && !concludiOperazione);
    
    return true;   
}

std::pair<std::string,bool> cambiaNome(Con2DB db, PGresult *res, int clientSocket, int USER_ID)
{
    char comando[1000];
    char buffer[1024] = {0};
    std::pair<std::string,bool> risultato;
    std::string request = "Inserisci il nuovo nome\n"; 
	int bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
    if (bytesRead > 0)
    {
        // Chiede all'utente il nuovo nome
        std::string messaggio(buffer, bytesRead);
        messaggio.erase(std::remove(messaggio.begin(), messaggio.end(), '\n'), messaggio.end()); // Rimuove eventuali newline
        if (messaggio.length() > 20 || messaggio.length() == 20)
        {                    
            std::string errore = "Il nome deve avere massimo 20 caratteri e minimo 0\n";
            send(clientSocket, errore.c_str(), errore.length(), 0);
            risultato.first = "";
            risultato.second = false;
            return risultato;
        }
        sprintf(comando, "UPDATE customers SET nome = '%s' WHERE id = %d", messaggio, USER_ID);
        db.ExecSQLcmd(res);
        risultato.first = messaggio;
        risultato.second = true;
        PQclear(res);
        return risultato;
    }
    std::string errore = "Input non valido\n";
    send(clientSocket, errore.c_str(), errore.length(), 0);
    risultato.first = "";
    risultato.second = false;
    return risultato
}

std::pair<std::string,bool> cambiaCognome(Con2DB db, PGresult *res, int clientSocket, int USER_ID)
{
    char comando[1000];
    char buffer[1024] = {0};
    std::pair<std::string,bool> risultato;
    std::string request = "Inserisci il nuovo cognome\n";
	int bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
    if (bytesRead > 0)
    {
        // Chiede all'utente il nuovo nome
        std::string messaggio(buffer, bytesRead);
        messaggio.erase(std::remove(messaggio.begin(), messaggio.end(), '\n'), messaggio.end()); // Rimuove eventuali newline
        if (messaggio.length() > 20 || messaggio.length() == 20)
        {                    
            std::string errore = "Il cognome deve avere massimo 20 caratteri e minimo 0\n";
            send(clientSocket, errore.c_str(), errore.length(), 0);
            risultato.first = "";
            risultato.second = false;
            return risultato;
        }
        sprintf(comando, "UPDATE customers SET cognome = '%s' WHERE id = %d", messaggio, USER_ID);
        db.ExecSQLcmd(res);
        risultato.first = messaggio;
        risultato.second = true;
        PQclear(res);
        return risultato;
    }
    std::string errore = "Input non valido\n";
    send(clientSocket, errore.c_str(), errore.length(), 0);
    risultato.first = "";
    risultato.second = false;
    return risultato
}