#include <cstring> 
#include <iostream> 
#include <netinet/in.h> 
#include <sys/socket.h> 
#include <unistd.h> 
  
using namespace std; 
  
int main() 
{ 
    // Crea il socket
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0); 
  
    // Specifica indirizzo del server 
    sockaddr_in serverAddress; 
    serverAddress.sin_family = AF_INET; 
    serverAddress.sin_port = htons(8080); 
    serverAddress.sin_addr.s_addr = INADDR_ANY; 
  
    //"Nome" del Server, permettendo a Socket di connettersi a lui
    // serverSocket = Socket NON associato
    // serverAddress = Struttura sockaddr a cui associare l'indirizzo del socket locale
    // sizeof(...) = Lunghezza (byte) del valore di serverAddress
    bind(serverSocket, (struct sockaddr*)&serverAddress, 
         sizeof(serverAddress)); 
  
    // Attende che un socket lo comunichi (finché non succede, server rimane in ascolto)
    // 5 = Lunghezza MASSIMA delle connessioni in sospeso
    listen(serverSocket, 5);

    // Accetta la connessione con il socket
    // serverSocket = Socket a cui verrà collegato
    // nullPtr 1 = (FACOLTATIVO) Indirizzo ad un buffer che riceve l'indirizz dell'entità di connessione
    // nullPtr 2 = (FACOLTATIVO) Specifica la lunghezza (int) del parametro nullPtr 2
    int clientSocket 
        = accept(serverSocket, nullptr, nullptr); 
  
    // Ricezione dati
    // buffer = Dati ricevuti dal Client verranno conservati qui
    char buffer[1024] = { 0 }; 
    recv(clientSocket, buffer, sizeof(buffer), 0); 
    cout << "Message from client: " << buffer 
              << endl;
    // Chiude il socket del server 
    close(serverSocket); 
  
    return 0; 
}
