#include "Customer_Server.h"

using namespace Pistache;

// Costruttore di Customer
Customer_Server::Customer_Server() {
    // Inizializzazione delle opzioni
    OPZIONI[0] = "Modifica nome e cognome";
    OPZIONI[1] = "Gestisci prodotti nel carrello";
    OPZIONI[2] = "Gestisci gli indirizzi registrati";
    OPZIONI[3] = "Aggiungi/Rimuovi prodotti da ordini";
    OPZIONI[4] = "Aggiungi/Rimuovi metodo di pagamento";

    // Effettua la connessione a Redis
    c2r = redisConnect(REDIS_IP, REDIS_PORT);
    if (c2r == nullptr || c2r->err) {
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

    // Inizializza Pistache in un thread separato
    pistacheThread = std::thread(&Customer_Server::startPistache, this);
}

void Customer_Server::defineRoutes() {
    // Bind the member function `handleOptions` to the instance of `Customer_Server`
    router.get("/", Pistache::Rest::Routes::bind(&Customer_Server::handleOptions, this));
}



void Customer_Server::handleOptions(const Rest::Request& request, Http::ResponseWriter response) {
    std::string responseMessage = "Opzioni disponibili:\n";
    for (int i = 0; i < NUMERO_OPZIONI; i++) {
        responseMessage += std::to_string(i + 1) + ") " + OPZIONI[i] + "\n";
    }
    responseMessage += "Oppure digita Q per terminare la connessione\n";
    response.send(Http::Code::Ok, responseMessage);
}

void Customer_Server::startPistache() {
    Http::Endpoint server(Address("localhost", SERVER_PORT));
    server.init(Http::Endpoint::options().threads(1).flags(Tcp::Options::ReuseAddr));
    defineRoutes();
    server.setHandler(router.handler());
    server.serve();
}

Customer_Server::~Customer_Server() {
    if (pistacheThread.joinable()) {
        pistacheThread.join();
    }
}

bool Customer_Server::handshake(int clientSocket) {
    // Questa funzione non sarà più utilizzata con Pistache
    return false;
}

void Customer_Server::gestisciConnessioneCliente(int clientSocket, int connectionID) {
    // Questa funzione non sarà più utilizzata con Pistache
}

bool Customer_Server::gestisciAutenticazione(int clientSocket) {
    // Questa funzione non sarà più utilizzata con Pistache
    return false;
}

bool Customer_Server::gestisciOperazioni(int clientSocket) {
    // Questa funzione non sarà più utilizzata con Pistache
    return false;
}

int main() {
    Customer_Server cst;
    // Non è più necessario gestire manualmente le connessioni
    return 0;
}
