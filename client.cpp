// C++ program to illustrate the client application in the 
// socket programming 
#include <cstring> 
#include <iostream> 
#include <netinet/in.h> 
#include <sys/socket.h> 
#include <unistd.h> 
  
int main() 
{ 
    // Crea un socket
    //AF_INET = Protocollo IPv4 (non ci interessa)
    //SOCK_STREAM = Specifica il tipo di Socket TCP (non ci interessa)
    //socketfd = File descriptor del socket (in caso di più socket,
    //necessario incrementarlo o creare buffer per conservarli
	int clientSocket = socket(AF_INET, SOCK_STREAM, 0); 

    // Specifica che server comunicare (nel caso si vogliano creare
    // creare un server per operazioni trasportatori, uno per produttori
    // e uno per cliente, conviene fare ServerTraspo = 1, 
    // ServerProdutto = 2 e ServerUtente = 3 o una cosa simile)
    // sockaddr_in è la struttura dati utilizzata per i dati del server
    // sin_family = Protocollo della connessione (AF_INET come sopra)
    // sin_port = Porta del protoccolo (Traspo = 1, Produt = 2, Utente = 3)
    // sin_addr = Ulteriore struttura dati che contiene un indirizzo di trasporto IPv4
    // htons(n) = Converte n da Machine Byte a Network Byte
    // INADDR_ANY = Usato se non si cercano particolari IP
	sockaddr_in serverAddress;
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(8080);
	serverAddress.sin_addr.s_addr = INADDR_ANY;

    // Richiesta di connessione
    // clientSocket = Socket da connettere
    // sockaddr = Socket a cui connetterlo
    	connect(clientSocket, (struct sockaddr*)&serverAddress,
	sizeof(serverAddress));

    // Invio dati
    // I dati accettati sono solo del tipo CONST CHAR*
    	const char* message = "Hello, server!";
    	send(clientSocket, message, strlen(message), 0);

    // Termina la connessione: close(socketDaChiudere)
    	close(clientSocket); 
    return 0; 
}
