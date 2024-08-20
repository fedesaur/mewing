#include "server.h"

Server::Server(const char* RedisIP, int RedisPort, int serverPort, const char* streamIN, const char* streamOUT)
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
	std::cout << "Connesso al client!" << std::endl;
    /*
        Effettua la connessione a Redis.
        (Per facilitare la lettura, l'ho spostato in un'altra funzione)
    */
    //ConnectToRedis(RedisIP, RedisPort, streamIN, streamOUT);
}
void Server::ConnectToRedis(const char* RedisIP, int RedisPort, const char* streamIN, const char* streamOUT)
{
    redisContext *c2r;
    redisReply *reply;
    c2r = redisConnect(RedisIP, RedisPort);
    // Se lo stream di lettura già esiste, lo cancella
    reply = RedisCommand(c2r, "DEL %s", streamIN);
    assertReply(c2r, reply);
    dumpReply(reply, 0);
    std::cout << "Stop S2"<< std::endl;

    // Se lo stream di scrttura già esiste, lo cancella
    reply = RedisCommand(c2r, "DEL %s", streamOUT);
    assertReply(c2r, reply);
    dumpReply(reply, 0);
    std::cout << "Stop S3"<< std::endl;

    initStreams(c2r, streamIN);
    initStreams(c2r, streamOUT);
    std::cout << "Stop S4"<< std::endl;

    READ_STREAM = streamIN;
    WRITE_STREAM = streamOUT;
	return;
}

void Server::Autenticazione(const char* PORT, const char* USERNAME, const char* PASSWORD)
{
    Con2DB db("localhost", PORT, USERNAME, PASSWORD, "mewingDB");
    //PGresult *res;
    redisContext *c2r=redisConnect("localhost",6379);
    redisReply *reply;
    char comando[1000];
    std::cout << "Stop S6"<< std::endl;

	reply = RedisCommand(c2r, "XGROUP GROUP %s autenticazione 0", WRITE_STREAM);
    assertReply(c2r, reply);
    freeReplyObject(reply);
	std::cout << "Gruppo per l'autenticazione creato";
    /*
        Con BLOCK, il server rimane in attesa per N tempo, sbloccandosi
        o quando scade il tempo o appena riceve un messaggio
    */
    reply = RedisCommand(c2r,
             "XREADGROUP GROUP autenticazione server BLOCK 6000 COUNT 1 STREAMS %s >",
			 READ_STREAM);
    assertReply(c2r, reply);
    std::cout << reply;
    dumpReply(reply, 0);
    freeReplyObject(reply);
    /*
        L'idea era di fare un tipo di autenticazione diversa per ogni tipo
        di utente (dato che per ciascuno deve essere effettuata una query diversa)
    */
    return;
}
