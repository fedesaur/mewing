#include "Courier_Server.h"

// Costruttore di Courier
Courier_Server::Courier_Server()
{
    // Definisce le opzioni del Supplier nel database
    OPZIONI[0] = "Modifica profilo";
    OPZIONI[1] = "Ricerca prodotti";
    OPZIONI[2] = "Ordina prodotti";
    OPZIONI[3] = "Aggiungi/Rimuovi prodotti da ordine";

    // Crea il socket del server
    SERVER_SOCKET = socket(AF_INET, SOCK_STREAM, 0); // Crea il socket
    if (SERVER_SOCKET < 0) {
        std::cerr << "Errore nella creazione del socket." << std::endl;
        exit(EXIT_FAILURE);
    }
    std::cout << "Socket del Server creato con successo: " << SERVER_SOCKET << std::endl;
    std::cout.flush();

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
    std::cout.flush();
    
    reply = RedisCommand(c2r, "DEL %s", READ_STREAM);
    assertReply(c2r, reply);
    freeReplyObject(reply);
    initStreams(c2r, READ_STREAM);
    std::cout << "Stream Read creato!" << std::endl;
    std::cout.flush();
}

void Courier_Server::gestisciConnessioni()
{
    // Attende che un socket si connetta (finché non succede, server rimane in ascolto)
    if (listen(SERVER_SOCKET, MAX_CONNECTIONS) < 0) {
        std::cerr << "Errore nell'ascolto sul socket." << std::endl;
        close(SERVER_SOCKET);
        exit(EXIT_FAILURE);
    }

    std::cout << "Server in ascolto sulla porta " << SERVER_PORT << "..." << std::endl;
    std::cout.flush();

    // Gestione delle connessioni dei client
    while (true)
    {
        int clientSocket = accept(SERVER_SOCKET, nullptr, nullptr);
        if (clientSocket < 0) {
            std::cerr << "Errore nell'accettare la connessione dal client." << std::endl;
            continue; // Continua ad accettare ulteriori connessioni
        }
        
        id_mutex.lock();
        int connectionID=ID_CONNESSIONE++;
        id_mutex.unlock();
        std::thread clientThread(&Courier_Server::gestisciConnessioneCliente, this, clientSocket, connectionID);
        clientThread.detach();

    }
    close(SERVER_SOCKET); // Chiudi il socket del server (questa parte non verrà mai raggiunta a causa del while infinito)
}

void Courier_Server::gestisciConnessioneCliente(int clientSocket, int connectionID){
        // Identifica l'ID della connessione
        bool continuaConnessione=false;
        std::cout << "ID della connessione: " + std::to_string(connectionID) << std::endl;
        std::cout.flush();
        std::string response = "ID della connessione: " + std::to_string(connectionID) + "\n";
        send(clientSocket, response.c_str(), response.length(), 0);
        bool connessioneOK = handshake(clientSocket); // Gestisci il client in una funzione dedicata
        if (connessioneOK && gestisciAutenticazione(clientSocket)) // Se la connessione è andata a buon fine, avvia le varie operazioni
        {
            // Lettura dello stream Redis
            reply = RedisCommand(c2r, "XREVRANGE %s + - COUNT 1", READ_STREAM);
            if (reply == nullptr || reply->type != REDIS_REPLY_ARRAY || reply->elements == 0)
            {
                std::cerr << "Errore nel comando Redis o stream vuoto" << std::endl;
            }

            redisReply* stream = reply->element[0];
            redisReply* entryFields = stream->element[1];
            
            if (entryFields->elements < 6) { // Assicurarsi che ci siano abbastanza campi
                std::cerr << "Errore: numero di campi insufficiente nello stream Redis." << std::endl;
                freeReplyObject(reply);
                
            }
            
            std::string ID = entryFields->element[1]->str; // ID Courier
            std::string piva = entryFields->element[3]->str; // piva
            std::string nome = entryFields->element[5]->str; // nome
            std::string indirizzo = entryFields -> element[7]->str; // Indirizzo (ID) Customer
            freeReplyObject(reply);

            if (ID.empty() || nome.empty() || piva.empty() || indirizzo.empty())
            {
                std::cerr << "Errore: non sono stati trovati nome o cognome con la chiave specificata." << std::endl;
            } else {
              std::string response = "Benvenuto " + nome + "\n";// Saluta il customer appena autenticato
              send(clientSocket, response.c_str(), response.length(), 0);
              
              /*
              CUSTOMER.ID = atoi(ID.c_str());
              CUSTOMER.nome = nome.c_str();
              CUSTOMER.cognome = cognome.c_str();
              CUSTOMER.mail = mail.c_str();
              CUSTOMER.abita = atoi(abita.c_str());
              */
              
              continuaConnessione = true;
            }

            // Andata a buon fine l'autenticazione, si rendono disponibile all'utente le varie funzionalità tramite una funzione ausiliaria
            do{
                continuaConnessione = gestisciOperazioni(clientSocket);
            }
            while (continuaConnessione);
            
        }

        close(clientSocket); // Chiudi la connessione con il client dopo averla gestita
        std::cout << "Conclusa connessione con ID: " + std::to_string(connectionID) << std::endl;
}

bool Courier_Server::handshake(int clientSocket) {
    char buffer[1024] = {0};
    std::string response = "Ciao\n";
    send(clientSocket, response.c_str(), response.length(), 0);

    int bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
    if (bytesRead > 0) return true;

    std::cerr << "Errore o nessun dato ricevuto dal client." << std::endl;
    return false;
}

bool Courier_Server::gestisciAutenticazione(int clientSocket)
{
    char buffer[1024] = {0};
    std::string request = "Inserisci la tua p.iva\n";
    send(clientSocket, request.c_str(), request.length(), 0);

    int bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
    if (bytesRead > 0) {
        // Chiede all'utente un nome utile all'identificazione
        std::string piva(buffer, bytesRead);
        std::string response = "P.iva ricevuta! Procedo all'autenticazione\n";
        send(clientSocket, response.c_str(), response.length(), 0);

        if (piva[piva.length()-1] == '\n') piva.erase(std::remove(piva.begin(), piva.end(), '\n'), piva.end()); // Rimuove \n alla fine dell'input
        // Scrive l'email ricevuta nello Stream
        reply = RedisCommand(c2r, "XADD %s * piva %s", WRITE_STREAM, piva.c_str());
        assertReplyType(c2r, reply, REDIS_REPLY_STRING);
        freeReplyObject(reply);
        return autentica(clientSocket); // Passa al processo di autenticazione
    }
    std::cerr << "Errore o nessun dato ricevuto dal client." << std::endl;
    return false;
}

bool Courier_Server::gestisciOperazioni(int clientSocket)
{
    char buffer[1024] = {0};
    std::string request = "Ecco le operazioni disponibili:\n";
    send(clientSocket, request.c_str(), request.length(), 0);

    // Legge le opzioni all'utente
    for (int i = 0; i < NUMERO_OPZIONI; i++)
    {
        std::string messaggio = std::to_string(i+1) + ") " + OPZIONI[i] + "\n";
        send(clientSocket, messaggio.c_str(), messaggio.length(), 0);
    }
    std::string termina = "Oppure digita Q per terminare la connessione\n";
    send(clientSocket, termina.c_str(), termina.length(), 0);
    
    bool attendiInput = true; // Continua la richiesta finché non riceve un input adatto

    do
    {
        int bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
        if (bytesRead > 0) {
            // Chiede all'utente un nome utile all'identificazione
            std::string messaggio(buffer, bytesRead);
            messaggio.erase(std::remove(messaggio.begin(), messaggio.end(), '\n'), messaggio.end()); // Rimuove eventuali newline

            if (messaggio == "q" || messaggio == "Q") {
                return false; // Termina la connessione
            }
            
            if (std::isdigit(messaggio[0])) {
                int opzione = std::stoi(messaggio) - 1;
                
                if (opzione >= 0 && opzione < NUMERO_OPZIONI) {
                    switch(opzione)
                    {
                        case 0:
                            std::cout << "Funzione Modifica profilo non implementata\n";
                            send(clientSocket, "Funzione non ancora implementata.\n", 35, 0);
                            break;
                        case 1:
                            cercaProdottiDisponibili(clientSocket);
                            break;
                        case 2:
                            std::cout << "Funzione Ordina prodotti non implementata\n";
                            send(clientSocket, "Funzione non ancora implementata.\n", 35, 0);
                            break;
                        case 3:
                            std::cout << "Funzione Aggiungi/Rimuovi prodotti da ordine non implementata\n";
                            send(clientSocket, "Funzione non ancora implementata.\n", 35, 0);
                            break;
                    }
                    attendiInput = false; // Input valido ricevuto, esce dal loop
                } else {
                    std::string errore = "Opzione non valida, riprova.\n";
                    send(clientSocket, errore.c_str(), errore.length(), 0);
                }
            } else {
                std::string errore = "Input non valido, riprova.\n";
                send(clientSocket, errore.c_str(), errore.length(), 0);
            }
        }
    } while (attendiInput); // Continua finché non riceve un input valido
    
    return true;
}


int main()
{
    /*
        Crea un server che si prepara ad avviare una
        connessione con l'utente
    */
    Courier_Server curry;
    curry.gestisciConnessioni();
    return 0;
}
