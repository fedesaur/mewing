#include "Customer.h"

// Costruttore di Customer
Customer::Customer()
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
    serverAddress.sin_port = htons(SERVER_PORT);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    // Associa l'indirizzo del server al socket
    if (bind(SERVER_SOCKET, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
        std::cerr << "Errore nel binding del socket." << std::endl;
        close(SERVER_SOCKET);
        exit(EXIT_FAILURE);
    }
}
    /* Effettuati controlli sui parametri per fare in modo che rispettino i limiti richiesti
    assert(nome.length() > 0 && nome.length() <= 20);
    assert(cognome.length() > 0 && cognome.length() <= 20);
    assert(mail.length() > 0 && mail.length() <= 50);

    Nome = nome;
    Cognome = cognome;
    Mail = mail;
    Abita = città;
void Customer::AggiungiIndirizzo(std::string via, int civico, std::string cap,std::string city,std::string stato)
{
    // Effettuati controlli sui parametri per fare in modo che rispettino i limiti richiesti
    assert(via.length() > 0 && via.length() <= 100);
    assert(civico >= 0);
    assert(cap.length() == 5);
    assert(city.length() > 0 && city.length() <= 30);
    assert(stato.length() > 0 && stato.length() <= 30);
}

void Customer::CreateSocket()
{
	redisContext *c2r; // c2r contiene le info sul contesto
	redisReply *reply; // reply contiene le risposte da Redis

	// Effettua la connessione a Redis
	c2r = redisConnect(REDIS_IP, REDIS_PORT);

	// In caso già esistano, elimina i due stream di lettura e scrittura
	reply = RedisCommand(c2r, "DEL %s", READ_STREAM);
	assertReply(c2r, reply);
	dumpReply(reply, 0);

	reply = RedisCommand(c2r, "DEL %s", WRITE_STREAM);
	assertReply(c2r, reply);
	dumpReply(reply, 0);

	// Crea gli stream per lettura e scrittura
	initStreams(c2r, READ_STREAM);
	initStreams(c2r, WRITE_STREAM);
	std::cout << "Stream Customer creati!" << std::endl;



	// Qui si tenta un processo di autenticazione tramite Redis
	reply = RedisCommand(c2r, "XADD %s * %s %s", WRITE_STREAM, "Mail", Mail.c_str());
	assertReplyType(c2r, reply, REDIS_REPLY_STRING);
	freeReplyObject(reply);
	std::cout << "Richiesta di autenticazione inviata!" << std::endl;

	// Finita la sua funzione, il socket viene chiuso

	return;
}
*/
void Customer::gestisciConnessioni()
{
	int IDConnessione = 0; // Serve giusto per annotare le connessioni accettate
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
        IDConnessione++;
        std::cout << "Accettata connessione numero: " + std::to_string(IDConnessione) << std::endl;
        std::string response = "Connessione numero: " + std::to_string(IDConnessione) + "\n";
        send(clientSocket, response.c_str(), response.length(), 0);

        // Gestisci il client in una funzione dedicata
        handleClient(clientSocket);
        close(clientSocket); // Chiudi la connessione con il client dopo averla gestita

    }
    // Chiudi il socket del server (questa parte non verrà mai raggiunta a causa del while infinito)
    close(SERVER_SOCKET);
}

void Customer::handleClient(int clientSocket) {
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
