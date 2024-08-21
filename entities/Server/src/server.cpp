#include "server.h"
#include <string>

Server::Server(const char* RedisIP, int RedisPort, int serverPort, const char* streamIN, const char* streamOUT)
{
    // Crea il socket
    SERVER_SOCKET = socket(AF_INET, SOCK_STREAM, 0);
	std::cout << "Socket del Server: " << SERVER_SOCKET << std::endl;

    // Specifica indirizzo del server
    sockaddr_in serverAddress;
    serverAddress.sin_port = htons(serverPort);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    //"Nome" del Server, permettendo a Socket di connettersi a lui
    // serverSocket = Socket NON associato
    // serverAddress = Struttura sockaddr a cui associare l'indirizzo del socket locale
    // sizeof(...) = Lunghezza (byte) del valore di serverAddress
    bind(SERVER_SOCKET, (struct sockaddr*)&serverAddress, sizeof(serverAddress));

    // Attende che un socket lo comunichi (finché non succede, server rimane in ascolto)
    // 5 = Lunghezza MASSIMA delle connessioni in sospeso
    listen(SERVER_SOCKET, 5);

    // Accetta la connessione con il socket
    // serverSocket = Socket a cui verrà collegato
    CLIENT_SOCKET = accept(SERVER_SOCKET, nullptr, nullptr);
	std::cout << "Connesso al client!" << std::endl;
	std::cout << "Socket del Client: " << CLIENT_SOCKET << std::endl;
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

void Server::handleClient(int clientSocket) {
    // Analizza il comando ricevuto
    std::string command(buffer);

    if (command.find("HELLO") == 0) {
        // Gestisci la richiesta di saluto
        std::string username = extractUsernameFromCommand(command); // Funzione per estrarre lo username dal comando

        // Controlla se l'utente esiste in Redis (ad esempio, utilizzando HGET)
        redisReply *reply = RedisCommand(c2r, "HGET users %s", username.c_str());
        if (reply && reply->str) {
            // Utente trovato, invia un saluto personalizzato
            std::string response = "Ciao, " + std::string(reply->str) + "!";
            send(clientSocket, response.c_str(), response.length(), 0);
        } else {
            // Utente non trovato, invia un saluto generico
            send(clientSocket, "Ciao! Benvenuto nel nostro negozio.", 34, 0);
        }
        freeReplyObject(reply);
    } else {
        // ... (gestisci altri comandi)
    }

    // ... (codice esistente)
}

std::string extractUsernameFromCommand(const std::string& command) {
    // Trova la posizione dello spazio dopo "HELLO"
    size_t pos = command.find(" ");
    
    // Se non c'è spazio, il comando è malformato
    if (pos == std::string::npos) {
        return "";
    }
    
    // Estrai lo username che segue lo spazio
    std::string username = command.substr(pos + 1);
    
    // Rimuovi eventuali spazi bianchi iniziali o finali dallo username (se necessario)
    username.erase(0, username.find_first_not_of(" \n\r\t"));
    username.erase(username.find_last_not_of(" \n\r\t") + 1);

    return username;
}
