#include "server.h"
#include <cstring>
#include <iostream>
#include <unistd.h> // per close()
#include <netinet/in.h> // per sockaddr_in e inet_addr

Server::Server(const char* RedisIP, int RedisPort, int serverPort, const char* streamIN, const char* streamOUT)
{
    // Crea il socket
    SERVER_SOCKET = socket(AF_INET, SOCK_STREAM, 0);
    if (SERVER_SOCKET < 0) {
        std::cerr << "Errore nella creazione del socket." << std::endl;
        exit(EXIT_FAILURE);
    }
    std::cout << "Socket del Server creato con successo: " << SERVER_SOCKET << std::endl;

    // Specifica indirizzo del server
    sockaddr_in serverAddress;
    memset(&serverAddress, 0, sizeof(serverAddress)); // Pulisce la struttura
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(serverPort);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    // Associa l'indirizzo del server al socket
    if (bind(SERVER_SOCKET, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
        std::cerr << "Errore nel binding del socket." << std::endl;
        close(SERVER_SOCKET);
        exit(EXIT_FAILURE);
    }

    // Attende che un socket si connetta (finché non succede, server rimane in ascolto)
    if (listen(SERVER_SOCKET, 5) < 0) {
        std::cerr << "Errore nell'ascolto sul socket." << std::endl;
        close(SERVER_SOCKET);
        exit(EXIT_FAILURE);
    }

    std::cout << "Server in ascolto sulla porta " << serverPort << "..." << std::endl;

    // Gestione delle connessioni dei client
    while (true) {
        int clientSocket = accept(SERVER_SOCKET, nullptr, nullptr);
        if (clientSocket < 0) {
            std::cerr << "Errore nell'accettare la connessione dal client." << std::endl;
            continue; // Continua ad accettare ulteriori connessioni
        }
        std::cout << "Connessione client accettata!" << std::endl;

        // Gestisci il client in una funzione dedicata
        handleClient(clientSocket);

        // Chiudi la connessione con il client dopo averla gestita
        close(clientSocket);
    }

    // Chiudi il socket del server (questa parte non verrà mai raggiunta a causa del while infinito)
    close(SERVER_SOCKET);
}

void Server::ConnectToRedis(const char* RedisIP, int RedisPort, const char* streamIN, const char* streamOUT)
{
    redisContext *c2r;
    redisReply *reply;

    c2r = redisConnect(RedisIP, RedisPort);
    if (c2r == nullptr || c2r->err) {
        std::cerr << "Errore nella connessione a Redis: " << (c2r ? c2r->errstr : "Connessione fallita") << std::endl;
        exit(EXIT_FAILURE);
    }

    // Se lo stream di lettura già esiste, lo cancella
    reply = RedisCommand(c2r, "DEL %s", streamIN);
    assertReply(c2r, reply);
    freeReplyObject(reply);

    // Se lo stream di scrittura già esiste, lo cancella
    reply = RedisCommand(c2r, "DEL %s", streamOUT);
    assertReply(c2r, reply);
    freeReplyObject(reply);

    // Inizializza gli stream
    initStreams(c2r, streamIN);
    initStreams(c2r, streamOUT);

    READ_STREAM = streamIN;
    WRITE_STREAM = streamOUT;
}

void Server::Autenticazione(const char* PORT, const char* USERNAME, const char* PASSWORD)
{
    Con2DB db("localhost", PORT, USERNAME, PASSWORD, "mewingDB");
    redisContext *c2r = redisConnect("localhost", 6379);
    redisReply *reply;

    reply = RedisCommand(c2r, "XGROUP CREATE %s autenticazione 0", WRITE_STREAM);
    assertReply(c2r, reply);
    freeReplyObject(reply);

    reply = RedisCommand(c2r, "XREADGROUP GROUP autenticazione server BLOCK 6000 COUNT 1 STREAMS %s >", READ_STREAM);
    assertReply(c2r, reply);
    freeReplyObject(reply);
}

void Server::handleClient(int clientSocket) {
    char buffer[1024] = {0};
    int bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);

    if (bytesRead > 0) {
        std::string response = "Ciao";
        send(clientSocket, response.c_str(), response.length(), 0);
    } else {
        std::cerr << "Errore o nessun dato ricevuto dal client." << std::endl;
    }
}

