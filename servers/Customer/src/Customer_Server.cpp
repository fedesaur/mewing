#include "Customer_Server.h"

// Costruttore di Customer
Customer_Server::Customer_Server()
{
    // Definisce le opzioni del Customer nel database
    OPZIONI[0] = "Modifica nome e cognome";
    OPZIONI[1] = "Aggiungi/Rimuovi prodotti dal carrello";
    OPZIONI[2] = "Gestisci gli indirizzi registrati";
    OPZIONI[3] = "Aggiungi/Rimuovi prodotti da ordini";
    OPZIONI[4] = "Aggiungi/Rimuovi metodo di pagamento";

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

void Customer_Server::gestisciConnessioni()
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
        
        
        std::thread clientThread(&Customer_Server::gestisciConnessioneCliente, this, clientSocket, connectionID);
        clientThread.detach();
    }
    close(SERVER_SOCKET); // Chiudi il socket del server (questa parte non verrà mai raggiunta a causa del while infinito)
}

void Customer_Server::gestisciConnessioneCliente (int clientSocket, int connectionID)
{
      // Identifica l'ID della connessione
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
            
            std::string ID = entryFields->element[1]->str; // ID Customer
            std::string nome = entryFields->element[3]->str; // Nome Customer
            std::string cognome = entryFields->element[5]->str; // Cognome ricevuto
            std::string mail = entryFields->element[7]->str; // Mail customer
            std::string abita = entryFields -> element[9]->str; // Indirizzo (ID) Customer
            freeReplyObject(reply);

            if (ID.empty() || nome.empty() || cognome.empty() || mail.empty())
            {
                std::cerr << "Errore: non sono stati trovati nome o cognome con la chiave specificata." << std::endl;
            } else {
              std::string response = "Benvenuto " + nome + " " + cognome + "\n";// Saluta il customer appena autenticato
              send(clientSocket, response.c_str(), response.length(), 0);
            }

            bool continuaConnessione=true;
            // Andata a buon fine l'autenticazione, si rendono disponibile all'utente le varie funzionalità tramite una funzione ausiliaria
            while (continuaConnessione) continuaConnessione = gestisciOperazioni(clientSocket);
        }
        close(clientSocket); // Chiudi la connessione con il client dopo averla gestita
        std::cout << "Conclusa connessione con ID: " + std::to_string(connectionID) << std::endl;

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

bool Customer_Server::gestisciAutenticazione(int clientSocket)
{
    char buffer[1024] = {0};
    std::string request = "Inserisci la tua email\n";
    send(clientSocket, request.c_str(), request.length(), 0);

    int bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
    if (bytesRead > 0) {
        // Chiede all'utente un nome utile all'identificazione
        std::string email(buffer, bytesRead);
        std::string response = "Email ricevuta! Procedo all'autenticazione\n";
        send(clientSocket, response.c_str(), response.length(), 0);

        email.erase(std::remove(email.begin(), email.end(), '\n'), email.end()); // Rimuove \n alla fine dell'input
        // Scrive l'email ricevuta nello Stream
        std::cout << email;
        reply = RedisCommand(c2r, "XADD %s * email %s", WRITE_STREAM, email.c_str());
        assertReplyType(c2r, reply, REDIS_REPLY_STRING);
        freeReplyObject(reply);
        return autentica(clientSocket); // Passa al processo di autenticazione
    }
    std::cerr << "Errore o nessun dato ricevuto dal client." << std::endl;
    return false;
}

bool Customer_Server::gestisciOperazioni(int clientSocket)
{
    char buffer[1024] = {0};
    std::string request = "\nEcco le operazioni disponibili:\n";
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
    bool esito = true;
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
                /*
    OPZIONI[0] = "Modifica nome e cognome";
    OPZIONI[1] = "Aggiungi/Rimuovi prodotti dal carrello";
    OPZIONI[2] = "Gestisci gli indirizzi registrati";
    OPZIONI[3] = "Aggiungi/Rimuovi prodotti da ordini";
    OPZIONI[4] = "Aggiungi/Rimuovi metodo di pagamento";
                */
                switch(opzione)
                {
                    case 0:
                        attendiInput = false; // Input valido ricevuto
                        esito = modificaNome(clientSocket); // Funziona!
                        break;
                    case 1:
                        attendiInput = false; // Input valido ricevuto
                        esito = ricercaProdotti(clientSocket);
                        break;
                    case 2:
                        attendiInput = false; // Input valido ricevuto, esce dal loop
                        esito = gestisciIndirizzi(clientSocket);
                        break;
                    case 3:
                        attendiInput = false; // Input valido ricevuto, esce dal loop
                        std::cout << "Funzione Aggiungi/Rimuovi prodotti da un ordine non implementata\n";
                        send(clientSocket, "Funzione non ancora implementata.\n", 35, 0);
                        break;
                    case 4:
                        attendiInput = false; // Input valido ricevuto, esce dal loop
                        esito = gestisciMetodi(clientSocket);
                        break;
                    default:
                        std::string errore = "Opzione non valida, riprova.\n";
                        send(clientSocket, errore.c_str(), errore.length(), 0);
                        break;
                }
            } else {
                std::string errore = "Input non valido, riprova.\n";
                send(clientSocket, errore.c_str(), errore.length(), 0);
            }
        }
    } while (attendiInput); // Continua finché non riceve un input valido
    
    return esito;
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
