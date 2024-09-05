#include "modificaInfoF.h"

bool modificaInfoF(int clientSocket)
{
    int PRODUCER_ID;
    PGresult *res;
    redisContext *c2r; // c2r contiene le info sul contesto
	redisReply *reply; // reply contiene le risposte da Redis
    char comando[1000];
    char buffer[1024] = {0};
    int NUMERO_OPZIONI = 5;
    std::string OPZIONI[] = {"1) Cambia Nome\n",
    "2)Cambia Partita IVA\n",
    "3)Cambia l'email\n",
    "4)Cambia il numero di telefono\n",
    "Altrimenti digita Q per terminare\n"};

	c2r = redisConnect(REDIS_IP, REDIS_PORT); // Effettua la connessione a Redis
	Con2DB db(HOSTNAME, DB_PORT, USERNAME, PASSWORD, DB_NAME); // Effettua la connessione al database

    reply = RedisCommand(c2r, "XREVRANGE %s + - COUNT 1", READ_STREAM);
    if (reply == nullptr || reply->type != REDIS_REPLY_ARRAY || reply->elements == 0)
    {
        std::cerr << "Errore nel comando Redis o stream vuoto" << std::endl;
        return false;
    }
    std::string id = reply->element[0]->element[1]->element[1]->str; 
    PRODUCER_ID = stoi(id); // ID Customer
    
    sprintf(comando, "SELECT * FROM fornitore WHERE id = %d", PRODUCER_ID);
    try
    {
        res = db.ExecSQLtuples(comando);
    
        // Recupera nome e cognome dell'utente dal database
        std::string nome = PQgetvalue(res, 0, PQfnumber(res, "nome"));
        std::string IVA = PQgetvalue(res, 0, PQfnumber(res, "iva"));
        std::string mail = PQgetvalue(res, 0, PQfnumber(res, "mail"));
        std::string telefono = PQgetvalue(res, 0, PQfnumber(res, "telefono"))
        std::string sede = PQgetvalue(res, 0, PQfnumber(res, "sede"));

        bool concludiOperazione = false;
        while (!concludiOperazione)
        {
            std::string request = "\nNome attuale: " + nome + " IVA Attuale: " + IVA + " Mail attuale: " + mail + " Telefono attuale: " + telefono + "\n"; 
	        send(clientSocket, request.c_str(), request.length(), 0);
            //Stampa all'utente le opzioni disponibili
            for (int i = 0; i < NUMERO_OPZIONI; i++) send(clientSocket, OPZIONI[i].c_str(), OPZIONI[i].length(), 0);
            int bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
            if (bytesRead > 0)
            {
                std::string messaggio(buffer, bytesRead);
                messaggio.erase(std::remove(messaggio.begin(), messaggio.end(), '\n'), messaggio.end()); // Rimuove eventuali newline

                if (messaggio == "q" || messaggio == "Q") {
                    concludiOperazione = true; // Termina la connessione
                } else if (std::isdigit(messaggio[0])) {
                    int opzione = std::stoi(messaggio) - 1;
                    std::pair<std::string,bool> risultato;
                    switch (opzione)
                    {
                    case 0:
                        risultato = cambiaNome(clientSocket, PRODUCER_ID);
                        if (risultato.second)
                        {
                            std::string conferma = "Nome cambiato!\n";
                            send(clientSocket, conferma.c_str(), conferma.length(), 0);
                            nome = risultato.first;
                        }
                        break;
                    case 1:
                        risultato = cambiaIVA(clientSocket, PRODUCER_ID);
                        if (risultato.second)
                        {   
                            std::string conferma = "IVA cambiata!\n";
                            send(clientSocket, conferma.c_str(), conferma.length(), 0);
                            IVA = risultato.first;
                        }        
                        break;
                    case 2:
                        risultato = cambiaMail(clientSocket, PRODUCER_ID);
                        if (risultato.second)
                        {   
                            std::string conferma = "Mail cambiata!\n";
                            send(clientSocket, conferma.c_str(), conferma.length(), 0);
                            IVA = risultato.first;
                        }        
                        break;
                    case 3:
                        risultato = cambiaTelefono(clientSocket, PRODUCER_ID);
                        if (risultato.second)
                        {   
                            std::string conferma = "Numero di telefono cambiato!\n";
                            send(clientSocket, conferma.c_str(), conferma.length(), 0);
                            IVA = risultato.first;
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
        PQclear(res);
        return true;   
    }
    catch(...)
    {
        std::string errore = "C'è stato un problema con il database\n";
        send(clientSocket, errore.c_str(), errore.length(), 0);
        return false;
    }
    
}

std::pair<std::string,bool> cambiaNome(int clientSocket, int producerID)
{
    char comando[1000];
    char buffer[1024] = {0};
    PGresult *res;
    Con2DB db(HOSTNAME, DB_PORT, USERNAME, PASSWORD, DB_NAME); // Effettua la connessione al database
    std::pair<std::string,bool> risultato;
    std::string request = "Inserisci il nuovo nome\n"; 
	int bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
    if (bytesRead > 0)
    {
        // Chiede all'utente il nuovo nome
        std::string messaggio(buffer, bytesRead);
        messaggio.erase(std::remove(messaggio.begin(), messaggio.end(), '\n'), messaggio.end()); // Rimuove eventuali newline
        if (messaggio.length() > 100 || messaggio.length() == 0)
        {                    
            std::string errore = "Il nome deve avere massimo 100 caratteri e minimo 1\n";
            send(clientSocket, errore.c_str(), errore.length(), 0);
            risultato.first = "";
            risultato.second = false;
            return risultato;
        }
        try
        {
            sprintf(comando, "UPDATE fornitore SET nome = '%s' WHERE id = %d", messaggio.c_str(), producerID);
            res = db.ExecSQLcmd(comando);
            risultato.first = messaggio;
            risultato.second = true;
            PQclear(res);
            return risultato;
        }
        catch(...)
        {
            std::string errore = "C'è stato un problema con il database\n";
            send(clientSocket, errore.c_str(), errore.length(), 0);
            risultato.first = "";
            risultato.second = false;
            PQclear(res);
            return risultato;
        }
    }
    std::string errore = "Input non valido\n";
    send(clientSocket, errore.c_str(), errore.length(), 0);
    risultato.first = "";
    risultato.second = false;
    return risultato;
}

std::pair<std::string,bool> cambiaIVA(int clientSocket, int producerID)
{
    char comando[1000];
    char buffer[1024] = {0};
    std::pair<std::string,bool> risultato;
    PGresult *res;
    Con2DB db(HOSTNAME, DB_PORT, USERNAME, PASSWORD, DB_NAME); // Effettua la connessione al database
    std::string request = "Inserisci la nuova partita IVA\n";
	int bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
    if (bytesRead > 0)
    {
        // Chiede all'utente il nuovo nome
        std::string messaggio(buffer, bytesRead);
        messaggio.erase(std::remove(messaggio.begin(), messaggio.end(), '\n'), messaggio.end()); // Rimuove eventuali newline
        if (messaggio.length() != 11 || !isNumber(messaggio))
        {                    
            std::string errore = "La partita IVA deve essere una sequenza di 11 cifre\n";
            send(clientSocket, errore.c_str(), errore.length(), 0);
            risultato.first = "";
            risultato.second = false;
            return risultato;
        }
        try
        {
            sprintf(comando, "UPDATE fornitore SET piva = '%s' WHERE id = %d", messaggio.c_str(), producerID);
            res = db.ExecSQLcmd(comando);
            risultato.first = messaggio;
            risultato.second = true;
            PQclear(res);
            return risultato;
        }
        catch(...)
        {
            std::string errore = "C'è stato un problema con il database\n";
            send(clientSocket, errore.c_str(), errore.length(), 0);
            risultato.first = "";
            risultato.second = false;
            PQclear(res);
            return risultato;
        }
    }
    std::string errore = "Input non valido\n";
    send(clientSocket, errore.c_str(), errore.length(), 0);
    risultato.first = "";
    risultato.second = false;
    return risultato;
}

std::pair<std::string,bool> cambiaMail(int clientSocket, int producerID)
{
    char comando[1000];
    char buffer[1024] = {0};
    PGresult *res;
    Con2DB db(HOSTNAME, DB_PORT, USERNAME, PASSWORD, DB_NAME); // Effettua la connessione al database
    std::pair<std::string,bool> risultato;
    std::string request = "Inserisci la nuova mail\n"; 
	int bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
    if (bytesRead > 0)
    {
        // Chiede all'utente il nuovo nome
        std::string messaggio(buffer, bytesRead);
        messaggio.erase(std::remove(messaggio.begin(), messaggio.end(), '\n'), messaggio.end()); // Rimuove eventuali newline
        if (messaggio.length() > 50 || messaggio.length() == 0)
        {                    
            std::string errore = "La mail deve avere massimo 50 caratteri e minimo 0\n";
            send(clientSocket, errore.c_str(), errore.length(), 0);
            risultato.first = "";
            risultato.second = false;
            return risultato;
        }
        try
        {
            sprintf(comando, "UPDATE fornitore SET mail = '%s' WHERE id = %d", messaggio.c_str(), producerID);
            res = db.ExecSQLcmd(comando);
            risultato.first = messaggio;
            risultato.second = true;
            PQclear(res);
            return risultato;
        }
        catch(...)
        {
            std::string errore = "C'è stato un problema con il database\n";
            send(clientSocket, errore.c_str(), errore.length(), 0);
            risultato.first = "";
            risultato.second = false;
            PQclear(res);
            return risultato;
        }
    }
    std::string errore = "Input non valido\n";
    send(clientSocket, errore.c_str(), errore.length(), 0);
    risultato.first = "";
    risultato.second = false;
    return risultato;
}

std::pair<std::string,bool> cambiaTelefono(int clientSocket, int producerID)
{
    char comando[1000];
    char buffer[1024] = {0};
    PGresult *res;
    Con2DB db(HOSTNAME, DB_PORT, USERNAME, PASSWORD, DB_NAME); // Effettua la connessione al database
    std::pair<std::string,bool> risultato;
    std::string request = "Inserisci il nuovo nome\n"; 
	int bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
    if (bytesRead > 0)
    {
        // Chiede all'utente il nuovo nome
        std::string messaggio(buffer, bytesRead);
        messaggio.erase(std::remove(messaggio.begin(), messaggio.end(), '\n'), messaggio.end()); // Rimuove eventuali newline
        if (messaggio.length() > 15|| messaggio.length() < 10 || !isNumber(messaggio))
        {                    
            std::string errore = "Il telefono deve essere una sequenza di massimo 15 cifre e minimo 10\n";
            send(clientSocket, errore.c_str(), errore.length(), 0);
            risultato.first = "";
            risultato.second = false;
            return risultato;
        }
        try
        {
            sprintf(comando, "UPDATE fornitore SET telefono = '%s' WHERE id = %d", messaggio.c_str(), producerID);
            res = db.ExecSQLcmd(comando);
            risultato.first = messaggio;
            risultato.second = true;
            PQclear(res);
            return risultato;
        }
        catch(...)
        {
            std::string errore = "C'è stato un problema con il database\n";
            send(clientSocket, errore.c_str(), errore.length(), 0);
            risultato.first = "";
            risultato.second = false;
            PQclear(res);
            return risultato;
        }
    }
    std::string errore = "Input non valido\n";
    send(clientSocket, errore.c_str(), errore.length(), 0);
    risultato.first = "";
    risultato.second = false;
    return risultato;
}