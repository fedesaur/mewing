#include "funzioniAusiliarie.h"
#include <algorithm>
#include <cctype>
#include <cstring>
#include <string>
#include <sys/socket.h>
#include <unistd.h> // for close()

bool isNumber(const std::string& s)
{
    return !s.empty() && std::all_of(s.begin(), s.end(), ::isdigit);
}

void rimuoviProdotto(int idP, Prodotto* insieme, int& righe)
{
    int index = 0;
    while (index < righe)
    {
        if (insieme[index].ID == idP) break; // Trova il prodotto con l'ID specificato
        index++;
    }
    
    if (index < righe)
    {
        for (int i = index; i < righe - 1; i++) 
        {
            insieme[i] = insieme[i + 1]; // Sposta gli elementi a sinistra
        }
        righe--; // Riduce il numero di elementi
    }
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
            if (isNumber(messaggio))
            {
                int numero = std::stoi(messaggio) - 1; // L'utente potrebbe inserire numeri a partire da 1
                if (numero >= 0 && numero < righe) 
                    return numero;
                else 
                {
                    std::string errore = "Input non valido\n";
                    send(clientSocket, errore.c_str(), errore.length(), 0); // Invia l'errore al client
                }
            } 
            else 
            {
                std::string errore = "Input non valido\n";
                send(clientSocket, errore.c_str(), errore.length(), 0); // Invia l'errore al client
            }
        }
        else 
        {
            std::string errore = "Input non valido, riprova.\n";
            send(clientSocket, errore.c_str(), errore.length(), 0);
            close(clientSocket); // Eventuale chiusura del socket in caso di errore persistente
            break;
        }
    }
    return indice;
}

void mostraOrdini(int clientSocket, int RIGHE, Ordine* ORDINI, Indirizzo* INDIRIZZI)
{
    if (RIGHE > 0)
    {
        std::string request = "\nORDINI REGISTRATI:\n";
        send(clientSocket, request.c_str(), request.length(), 0); // Invia il messaggio pre-impostato all'utente

        for (int i = 0; i < RIGHE; i++)
        {
            // Recupera gli attributi degli ordini registrati
            int ID = ORDINI[i].ID;
            std::string mail = ORDINI[i].MailCustomer;
            std::string data = ORDINI[i].DataRichiesta;
            std::string statoOrd = ORDINI[i].Stato;
            std::string paga = ORDINI[i].Pagamento;
            double totale = ORDINI[i].Totale;

            std::string via = INDIRIZZI[i].via;
            std::string civico = INDIRIZZI[i].civico;
            std::string CAP = INDIRIZZI[i].CAP;
            std::string citta = INDIRIZZI[i].citta;
            std::string stato = INDIRIZZI[i].stato;

            // Componi il messaggio dell'ordine
            std::string ordine = std::to_string(i + 1) + ") ID Ordine: " + std::to_string(ID) +
             " Mail Customer: " + mail + 
             " Data Richiesta: " + data + 
             " Stato Ordine: " + statoOrd +
             " Metodo Pagamento: " + paga +
             " Totale Ordine: " + std::to_string(totale) + 
             " Indirizzo: " + via + " " + civico + ", " + CAP + " " + citta + ", " + stato + "\n";

            send(clientSocket, ordine.c_str(), ordine.length(), 0); // Invia l'ordine al client
        }
    }
    else
    {
        std::string vuoto = "Non ci sono ordini registrati!\n";
        send(clientSocket, vuoto.c_str(), vuoto.length(), 0); // Invia la frase all'utente
    }
}
