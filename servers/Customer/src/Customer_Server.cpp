#include "Customer_Server.h"

// Costruttore di Customer
Customer_Server::Customer_Server()
{
    SERVER_SOCKET = socket(AF_INET, SOCK_STREAM, 0); // Crea il socket
    if (SERVER_SOCKET < 0) {
        std::cerr << "Errore nella creazione del socket." << std::endl;
        exit(EXIT_FAILURE);
    }
    std::cout << "Socket del Server creato con successo: " << SERVER_SOCKET << std::endl;

    // Specifica indirizzo del server
    sockaddr_in serverAddress;
    memset(&serverAddress, 0, sizeof(serverAddress)); // Pulisce la struttura
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(SERVER_PORT);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    // Associa l'indirizzo del server al socket
    if (bind(SERVER_SOCKET, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
        std::cerr << "Errore nel binding del socket." << std::endl;
        close(SERVER_SOCKET);
        exit(EXIT_FAILURE);
    }

    // Effettua la connessione a Redis
    c2r = redisConnect(REDIS_IP, REDIS_PORT);
    if (c2r == nullptr || c2r->err)
    {
        std::cerr << "Errore nella connessione a Redis: " << c2r->errstr << std::endl;
        exit(EXIT_FAILURE);
    }

    // Viene creato lo Stream Redis per il trasferimento di messaggi
    reply = RedisCommand(c2r, "DEL %s", WRITE_STREAM);
    assertReply(c2r, reply);
    freeReplyObject(reply);
    initStreams(c2r, WRITE_STREAM);
    std::cout << "Stream Write creato!" << std::endl;
    
    reply = RedisCommand(c2r, "DEL %s", READ_STREAM);
    assertReply(c2r, reply);
    freeReplyObject(reply);
    initStreams(c2r, READ_STREAM);
    std::cout << "Stream Read creato!" << std::endl;
}

void Customer_Server::gestisciConnessioni()
{
    // Attende che un socket si connetta (finché non succede, server rimane in ascolto)
    if (listen(SERVER_SOCKET, MAX_CONNECTIONS) < 0) {
        std::cerr << "Errore nell'ascolto sul socket." << std::endl;
        close(SERVER_SOCKET);
        exit(EXIT_FAILURE);
    }

    std::cout << "Server in ascolto sulla porta " << SERVER_PORT << "..." << std::endl;

    // Gestione delle connessioni dei client
    while (true)
    {
        int clientSocket = accept(SERVER_SOCKET, nullptr, nullptr);
        if (clientSocket < 0) {
            std::cerr << "Errore nell'accettare la connessione dal client." << std::endl;
            continue; // Continua ad accettare ulteriori connessioni
        }

		// Identifica l'ID della connessione
        std::cout << "ID della connessione: " + std::to_string(ID_CONNESSIONE) << std::endl;
        std::string response = "ID della connessione: " + std::to_string(ID_CONNESSIONE) + "\n";
        send(clientSocket, response.c_str(), response.length(), 0);

        bool connessioneOK = handshake(clientSocket); // Gestisci il client in una funzione dedicata
        if (connessioneOK && authenticate(clientSocket)) // Se la connessione è andata a buon fine, avvia le varie operazioni
        {
            autentica(clientSocket); // Passa al processo di autenticazione
             reply = RedisCommand(c2r, "XREVRANGE %s + - COUNT 1", READ_STREAM);
    if (reply == nullptr || reply->type != REDIS_REPLY_ARRAY || reply->elements == 0)
	{
       std::cerr << "Errore nel comando Redis o stream vuoto" << std::endl;
       return;
    }

    redisReply* stream = reply -> element[0];
    redisReply* entryFields = stream -> element[1];
    std::string fieldName = entryFields->element[0]->str; // Chiave
    std::string received_email = entryFields->element[1]->str; // Valore
    freeReplyObject(reply);

    if (received_email.empty())
      {
	  std::cerr << "Errore: non è stata trovata nessuna email con la chiave specificata." << std::endl;
	  return; 
      }
    received_email.pop_back();
    std::cout << "Email letta dallo stream: " << received_email << std::endl;
    const char* mail = received_email.c_str();

        }
        close(clientSocket); // Chiudi la connessione con il client dopo averla gestita
        std::cout << "Conclusa connessione con ID: " + std::to_string(ID_CONNESSIONE) << std::endl;
		ID_CONNESSIONE++;
    }
    // Chiudi il socket del server (questa parte non verrà mai raggiunta a causa del while infinito)
    close(SERVER_SOCKET);
}

bool Customer_Server::handshake(int clientSocket) {
    char buffer[1024] = {0};
    std::string response = "Ciao\n";
    send(clientSocket, response.c_str(), response.length(), 0);

    int bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
    if (bytesRead > 0) return true;

    std::cerr << "Errore o nessun dato ricevuto dal client." << std::endl;
    return false;
}

bool Customer_Server::authenticate(int clientSocket)
{
    char buffer[1024] = {0};
    std::string request = "Inserisci la tua email\n";
    send(clientSocket, request.c_str(), request.length(), 0);

    int bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
    if (bytesRead > 0) {
		//Chide all'utente una mail utile all'identificazione
        std::string email(buffer, bytesRead);
        std::string response = "Email ricevuta! Procedo all'autenticazione\n";
        send(clientSocket, response.c_str(), response.length(), 0);

        // Scrive la mail ricevuta nello Stream
        reply = RedisCommand(c2r, "XADD %s * %s %s", WRITE_STREAM, std::to_string(ID_CONNESSIONE).c_str(), email.c_str());
        assertReplyType(c2r, reply, REDIS_REPLY_STRING);
        freeReplyObject(reply);
        return true;
    }
    std::cerr << "Errore o nessun dato ricevuto dal client." << std::endl;
    return false;
}


int main()
{
    /*
        Crea un server che si prepara ad avviare una
        connessione con l'utente
    */
    Customer_Server cst;
    cst.gestisciConnessioni();
    return 0;
}
