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
                if (numero >= 0 && numero < righe) indice = numero;
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