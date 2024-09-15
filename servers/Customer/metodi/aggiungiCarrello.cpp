#include "aggiungiCarrello.h"

bool aggiungiCarrello(int clientSocket, int customerID, int RIGHE, Prodotto* PRODOTTI)
{
    char buffer[1024] = {0};
    try
    {
        if (RIGHE > 0)
        {
            bool terminaConnessione = false;
            while (!terminaConnessione)
            {
                mostraProdotti(clientSocket, PRODOTTI, RIGHE);
                std::string request = "\nQuale prodotto vuoi aggiungere al carrello? (Digita il numero)\nOppure digita Q per terminare la connessione\n";
	            send(clientSocket, request.c_str(), request.length(), 0); // Invia il messaggio pre-impostato all'utente
                int bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
                if (bytesRead > 0) 
                {
                    std::string messaggio(buffer, bytesRead);
                    messaggio.erase(std::remove(messaggio.begin(), messaggio.end(), '\n'), messaggio.end()); // Rimuove eventuali newline

                    if (messaggio == "q" || messaggio == "Q") terminaConnessione = false;
                    else if (isNumber(messaggio))
                    {
                        int indice = stoi(messaggio) - 1;
                        if (indice >= 0 && indice < RIGHE)
                        {
                            int idP = prodottiDisponibili[indice].ID;
                            int quantita = richiediQuantita(clientSocket); // Chiede la quantita di prodotto desiderata dall'utente
                            bool esito = aggiungiCarrelloDB(idP, customerID, quantita); // Aggiunge il prodotto al carrello
                            if (esito)
                            {
                                std::string successo = "Prodotto aggiunto al carrello con successo!\n\n";
                                send(clientSocket, successo.c_str(), successo.length(), 0);
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
            PQclear(res);
            return true;
        }   
        // Se non ci sono oggetti
        PQclear(res);
	    std::string request = "Nessun prodotto disponibile!\n\n"; // Seleziona la frase del turno
	    send(clientSocket, request.c_str(), request.length(), 0); // Invia il messaggio pre-impostato all'utente
        return false;
    }
    catch(...)
    {
        std::string errore = "C'è stato un errore nel database!\n"; // Seleziona la frase del turno
	    send(clientSocket, errore.c_str(), errore.length(), 0); // Invia il messaggio pre-impostato all'utente
        return false; // Controlla che la query sia andata a buon fine
    }
}

int richiediQuantita(int clientSocket)
{
    int quantita = -1;
    char buffer[1024] = {0};
    std::string request = "In che quantita ne vuoi?\n";
	send(clientSocket, request.c_str(), request.length(), 0); // Invia il messaggio pre-impostato all'utente
    while (quantita == -1)
    {
        int bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
        if (bytesRead > 0)
        {
            std::string messaggio(buffer, bytesRead);
            messaggio.erase(std::remove(messaggio.begin(), messaggio.end(), '\n'), messaggio.end()); // Rimuove eventuali newline
            if (isNumber(messaggio)) 
            {
                int numero = stoi(messaggio);
                if (numero > 0) {
                    quantita = numero;
                } else {
                    std::string errore = "Quantità non valida\n";
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
  return quantita;
}

bool aggiungiCarrelloDB(int idProdotto, int userID, int quantita)
{
    char comando[1000];
    int rows;
    PGresult *res;
    Con2DB db(HOSTNAME, DB_PORT, USERNAME_CUST, PASSWORD_CUST, DB_NAME); // Effettua la connessione al database
    sprintf(comando, "SELECT quantita FROM prodincart WHERE prodotto = %d AND carrello = %d", idProdotto, userID);
    try
    {
        res = db.ExecSQLtuples(comando);
        rows = PQntuples(res);
        if (rows == 1) // Il prodotto già c'è, perciò aumenta il numero di prodotti presenti
        {
            int plus = atoi(PQgetvalue(res, 0, PQfnumber(res, "quantita"))) + quantita;
            sprintf(comando, "UPDATE prodincart SET quantita = %d WHERE prodotto = %d AND carrello = %d", plus, idProdotto, userID);
        }
        else {sprintf(comando, "INSERT INTO prodincart(carrello, prodotto, quantita) VALUES (%d, %d, %d)", userID, idProdotto, quantita);}
        res = db.ExecSQLcmd(comando);
        PQclear(res);
        return true;
    }
    catch(...)
    {
        std::string errore = "C'è stato un errore nel database!\n"; // Seleziona la frase del turno
	    send(clientSocket, errore.c_str(), errore.length(), 0); // Invia il messaggio pre-impostato all'utente
        return false;
    }
}
