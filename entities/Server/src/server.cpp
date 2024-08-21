#include "server.h"
#include <cstring>
#include <iostream>
#include <unistd.h> // per close()
#include <netinet/in.h> // per sockaddr_in e inet_addr

Server::Server(int serverPort)
{

}

void Server::handleClient(int clientSocket) {
    char buffer[1024] = {0};
    int bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);

    if (bytesRead > 0) {
        std::string response = "Ciao\n";
        send(clientSocket, response.c_str(), response.length(), 0);
        return;
    }
    std::cerr << "Errore o nessun dato ricevuto dal client." << std::endl;
    return;
}

/*
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
    PGresult *res; //Conserva i risultati delle query

    reply = RedisCommand(c2r, "XGROUP CREATE %s autenticazione 0", WRITE_STREAM);
    assertReply(c2r, reply);
    freeReplyObject(reply);

    reply = RedisCommand(c2r, "XREADGROUP GROUP autenticazione server BLOCK 6000 COUNT 1 STREAMS %s >", READ_STREAM);
    assertReply(c2r, reply);
    freeReplyObject(reply);
}
*/

