#include "Supplier_Server.h"

using namespace Pistache;

// Costruttore di Courier
Supplier_Server::Supplier_Server()
{
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
    std::cout.flush();
    
    reply = RedisCommand(c2r, "DEL %s", READ_STREAM);
    assertReply(c2r, reply);
    freeReplyObject(reply);
    initStreams(c2r, READ_STREAM);
    std::cout << "Stream Read creato!" << std::endl;
    std::cout.flush();

    pistacheThread = std::thread(&Courier_Server::startPistache, this);
}

void Supplier_Server::defineRoutes() {
    std::cout << "Registrazione delle rotte..." << std::endl;
    ::defineRoutes(router);
    std::cout << "Rotte registrate con successo!" << std::endl;
}

void Supplier_Server::startPistache() {
    Http::Endpoint server(Address("localhost", 5003));
    server.init(Http::Endpoint::options().threads(1).flags(Tcp::Options::ReuseAddr));
    defineRoutes();
    server.setHandler(router.handler());
    server.serve();
}

Supplier_Server::~Supplier_Serverr() {
    if (pistacheThread.joinable()) {
        pistacheThread.join();
    }
}

int main() 
{
    /*
        Crea un server che si prepara ad avviare una
        connessione con l'utente
    */
    Supplier_Server suppli;
    return 0;
}