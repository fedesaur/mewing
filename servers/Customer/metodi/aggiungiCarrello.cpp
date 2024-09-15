#include "aggiungiCarrello.h"

bool aggiungiCarrello(int clientSocket, int customerID, Prodotto* prodotti, int RIGHE)
{
    char buffer[1024] = {0};
    
    if (RIGHE <= 0) {
        std::string request = "Nessun prodotto disponibile!\n";
        send(clientSocket, request.c_str(), request.length(), 0);
        return false;
    }

    bool terminaConnessione = false;
    while (!terminaConnessione)
    {
        mostraProdotti(clientSocket, prodotti, RIGHE);
        std::string request = "\nQuale prodotto vuoi aggiungere al carrello? (Digita il numero)\nOppure digita Q per terminare la connessione\n";
        send(clientSocket, request.c_str(), request.length(), 0);
        
        int bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
        if (bytesRead > 0)
        {
            std::string messaggio(buffer, bytesRead);
            messaggio.erase(std::remove(messaggio.begin(), messaggio.end(), '\n'), messaggio.end());

            if (messaggio == "q" || messaggio == "Q") {
                terminaConnessione = true;
            }
            else if (isNumber(messaggio)) {
                int indice = stoi(messaggio) - 1;
                if (indice >= 0 && indice < RIGHE) {
                    int idP = prodotti[indice].ID;
                    int quantita = richiediQuantita(clientSocket); 
                    bool esito = aggiungiCarrelloDB(idP, customerID, quantita);
                    if (esito) {
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
        } else {
            std::string errore = "Errore di input, riprova.\n\n";
            send(clientSocket, errore.c_str(), errore.length(), 0);
        }
    }
    
    return true;
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
    PGresult *res;
    Con2DB db(HOSTNAME, DB_PORT, USERNAME_CUST, PASSWORD_CUST, DB_NAME); // Connessione DB

    try
    {
        sprintf(comando, "SELECT quantita FROM prodincart WHERE prodotto = %d AND carrello = %d", idProdotto, userID);
        res = db.ExecSQLtuples(comando);
        
        int rows = PQntuples(res);
        if (rows == 1) {
            int plus = atoi(PQgetvalue(res, 0, PQfnumber(res, "quantita"))) + quantita;
            sprintf(comando, "UPDATE prodincart SET quantita = %d WHERE prodotto = %d AND carrello = %d", plus, idProdotto, userID);
        } else {
            sprintf(comando, "INSERT INTO prodincart(carrello, prodotto, quantita) VALUES (%d, %d, %d)", userID, idProdotto, quantita);
        }

        PQclear(res);
        res = db.ExecSQLcmd(comando);
        PQclear(res);

        return true;
    }
    catch(...)
    {
        // Gestione errori database
        return false;
    }
}
