#include "Customer.h"

// Costruttore di Customer
Customer::Customer()
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
    if (c2r == nullptr || c2r->err) {
        std::cerr << "Errore nella connessione a Redis: " << c2r->errstr << std::endl;
        exit(EXIT_FAILURE);
    }

    creaStreams(); // Vengono creati gli Streams Redis per il trasferimento di messaggi
}

void Customer::creaStreams()
{
    // In caso già esistano, elimina i due stream di lettura e scrittura
    reply = RedisCommand(c2r, "DEL %s", READ_STREAM);
    assertReply(c2r, reply);
    freeReplyObject(reply);

    reply = RedisCommand(c2r, "DEL %s", WRITE_STREAM);
    assertReply(c2r, reply);
    freeReplyObject(reply);

    // Crea gli stream per lettura e scrittura
    initStreams(c2r, READ_STREAM);
    initStreams(c2r, WRITE_STREAM);
    std::cout << "Stream Customer creati!" << std::endl;
}

void Customer::gestisciConnessioni()
{
    CONNESSIONI_RICEVUTE = 0; // Serve giusto per annotare le connessioni accettate
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
        CONNESSIONI_RICEVUTE++;
        std::cout << "Accettata connessione numero: " + std::to_string(CONNESSIONI_RICEVUTE) << std::endl;
        std::string response = "Connessione numero: " + std::to_string(CONNESSIONI_RICEVUTE) + "\n";
        send(clientSocket, response.c_str(), response.length(), 0);

        bool connessioneOK = handshake(clientSocket); // Gestisci il client in una funzione dedicata
        if (connessioneOK && authenticate(clientSocket)) // Se la connessione è andata a buon fine, avvia le varie operazioni
        {
            //autentica(); // Chiama l'autenticazione
            //metti codice che consente all'utente di continuare a mandare messaggi fino a che non scrive quit
            
        }
        close(clientSocket); // Chiudi la connessione con il client dopo averla gestita
        std::cout << "Conclusa connessione numero: " + std::to_string(CONNESSIONI_RICEVUTE) << std::endl;
    }
    // Chiudi il socket del server (questa parte non verrà mai raggiunta a causa del while infinito)
    close(SERVER_SOCKET);
}

bool Customer::handshake(int clientSocket) {
    char buffer[1024] = {0};
    int bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);

    if (bytesRead > 0) {
        std::string response = "Ciao\n";
        send(clientSocket, response.c_str(), response.length(), 0);
        return true;
    }
    std::cerr << "Errore o nessun dato ricevuto dal client." << std::endl;
    return false;
}

bool Customer::authenticate(int clientSocket)
{
    char buffer[1024] = {0};
    std::string request = "Inserisci la tua email\n";
    send(clientSocket, request.c_str(), request.length(), 0);

    int bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
    if (bytesRead > 0) {
        std::string email(buffer, bytesRead);
        std::string response = "Email ricevuta! Procedo all'autenticazione\n";
        send(clientSocket, response.c_str(), response.length(), 0);

        // Scrive sullo stream l'email dell'utente per l'autenticazione
        std::cout << std::to_string(CONNESSIONI_RICEVUTE) << std::endl;
        const char* chiave = std::to_string(CONNESSIONI_RICEVUTE).c_str();
        reply = RedisCommand(c2r, "XADD %s * %s %s", WRITE_STREAM, chiave, email.c_str());
        assertReplyType(c2r, reply, REDIS_REPLY_STRING);
        freeReplyObject(reply);

        /*
         Legge lo stream per verificare l'autenticazione
        */
        reply = RedisCommand(c2r, "XREAD COUNT 1000 STREAMS %s 0", WRITE_STREAM);
        if (reply == nullptr || reply->type != REDIS_REPLY_ARRAY || reply->elements == 0) {
            std::cerr << "Errore nel comando Redis o stream vuoto" << std::endl;
            return false;
        }

        // Estrarre l'email dal reply
        // int risultato = ReadNumStreams(reply);
        redisReply* stream = reply->element[0]->element[1]->element[0];
        redisReply* email_entry = stream->element[1]->element[1];
        std::string received_email = email_entry->str;

        std::cout << "Email letta dallo stream: " << received_email << std::endl;
        RedisCommand(c2r, "XTRIM %s MAXLEN 1000", WRITE_STREAM);
        freeReplyObject(reply);

        return autentica();
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
    Customer cst;
    cst.gestisciConnessioni();
    return 0;
}
