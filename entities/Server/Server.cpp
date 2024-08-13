#include "Server.h"

Server::Server(char* RedisIP, int RedisPort, int serverPort, char* streamIN, char* streamOUT)
{
    // Crea il socket
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    // Specifica indirizzo del server
    sockaddr_in serverAddress;
    serverAddress.sin_port = htons(serverPort);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    //"Nome" del Server, permettendo a Socket di connettersi a lui
    // serverSocket = Socket NON associato
    // serverAddress = Struttura sockaddr a cui associare l'indirizzo del socket locale
    // sizeof(...) = Lunghezza (byte) del valore di serverAddress
    bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress));

    // Attende che un socket lo comunichi (finché non succede, server rimane in ascolto)
    // 5 = Lunghezza MASSIMA delle connessioni in sospeso
    listen(serverSocket, 5);

    // Accetta la connessione con il socket
    // serverSocket = Socket a cui verrà collegato
    int clientSocket = accept(serverSocket, nullptr, nullptr);

    /*
        Effettua la connessione a Redis.
        (Per facilitare la lettura, l'ho spostato in un'altra funzione)
    */
    ConnectToRedis(RedisIP, RedisPort, streamIN, streamOUT);
    Autenticazione(serverPort);
}
void Server::ConnectToRedis(char* RedisIP, int RedisPort, char* streamIN, char* streamOUT)
{

    c2r = redisConnect(RedisIP, RedisPort);
    // Se lo stream di lettura già esiste, lo cancella
    reply = RedisCommand(c2r, "DEL %s", streamIN);
    assertReply(c2r, reply);
    dumpReply(reply, 0);

    // Se lo stream di scrttura già esiste, lo cancella
    reply = RedisCommand(c2r, "DEL %s", streamOUT);
    assertReply(c2r, reply);
    dumpReply(reply, 0);

    initStreams(c2r, streamIN);
    initStreams(c2r, streamOUT);

    READ_STREAM = streamIN;
    WRITE_STREAM = streamOUT;
}

void Server::Autenticazione(int serverPort)
{
    reply = RedisCommand(c2r,
             "XREADGROUP GROUP main server BLOCK 0 COUNT 1 NOACK STREAMS %s >",
			 READ_STREAM);
    assertReply(c2r, reply);
    dumpReply(reply, 0);
    freeReplyObject(reply);


}
