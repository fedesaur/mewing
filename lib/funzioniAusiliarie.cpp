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
