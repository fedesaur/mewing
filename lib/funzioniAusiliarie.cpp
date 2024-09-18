#include "funzioniAusiliarie.h"

bool isNumber(std::string s)
{
    std::string::const_iterator it = s.begin();
    while (it != s.end() && std::isdigit(*it)) ++it;
    return !s.empty() && it == s.end();
}

void rimuoviProdotto(int idP, Prodotto* insieme, int righe)
{
    int index = 0;
    while (index < righe)
    {
        if (insieme[index].ID == idP) break; //Rimuove dal carrello il prodotto con quell'ID
        index++;
    }
    
    while (index < righe) insieme[index] = insieme[index+1]; //Sposta tutti gli elementi di una posizione a sinistra
    return;
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
                if (numero >= 0 && numero < righe) return numero;
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


void mostraOrdini(int clientSocket, int RIGHE, Ordine* ORDINI, Indirizzo* INDIRIZZI)
{
    if (RIGHE > 0)
    {
        std::string request = "\nORDINI REGISTRATI:\n"; //... e lo stampa
	    send(clientSocket, request.c_str(), request.length(), 0); // Invia il messaggio pre-impostato all'utente
        for (int i = 0; i < RIGHE; i++)
        {
            // Recupera gli attributi degli ordini registrati...
            int ID = ORDINI[i].ID;
                ordiniDisponibili[i].MailCustomer = mail;
                ordiniDisponibili[i].DataRichiesta = time;
                ordiniDisponibili[i].Stato = statoOrd;
                ordiniDisponibili[i].Pagamento = paga;
                ordiniDisponibili[i].Totale = totale;

                indirizzoOrdini[i].via = via;
                indirizzoOrdini[i].civico = civico;
                indirizzoOrdini[i].CAP = CAP;
                indirizzoOrdini[i].citta = city;
                indirizzoOrdini[i].stato = stato;
            // ...e li invia all'utente così che possa visualizzarli ed effettuarci operazioni
            std::string ordine = std::to_string(i+1) + ") ID Ordine: " + std::to_string(ID) +
             " Mail Customer: " + mail + 
             " Data Richiesta: " + std::to_string(data) + 
             " Metodo Pagamento: " + paga +
             " Totale Ordine: " + std::to_string(totale) + "\n";
	        send(clientSocket, ordine.c_str(), ordine.length(), 0);
        }
    } else {
        std::string vuoto = "Non ci sono ordini registrati!\n";
        send(clientSocket, vuoto.c_str(), vuoto.length(), 0); // Invia la frase all'utente
    }
    return;
}