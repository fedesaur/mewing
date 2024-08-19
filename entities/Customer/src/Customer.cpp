#include "Customer.h"

// Costruttore di Customer
Customer::Customer(std::string nome, std::string cognome,std::string mail,int città)
{
    // Effettuati controlli sui parametri per fare in modo che rispettino i limiti richiesti
    assert(nome.length() > 0 && nome.length() <= 20);
    assert(cognome.length() > 0 && cognome.length() <= 20);
    assert(mail.length() > 0 && mail.length() <= 50);

    Nome = nome;
    Cognome = cognome;
    Mail = mail;
    Abita = città;
}
void Customer::AggiungiIndirizzo(std::string via, int civico, std::string cap,std::string city,std::string stato)
{
    // Effettuati controlli sui parametri per fare in modo che rispettino i limiti richiesti
    assert(via.length() > 0 && via.length() <= 100);
    assert(civico >= 0);
    assert(cap.length() == 5);
    assert(city.length() > 0 && city.length() <= 30);
    assert(stato.length() > 0 && stato.length() <= 30);
}

void ConnectToServer()
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

	/*
		OVERCOMPLICATED: Qui creo un processo figlio perché la ricezione delle informazioni
		per l'autenticazione è BLOCCANTE, quindi il processo rimarrebbe in attesa delle
		informazioni per N tempo, ma le informazioni non arriverebbero mai, quindi
		ho preferito questa opzione:
		PADRE: Crea il server e attende le informazioni dal figlio
		FIGLIO: Invia le informazioni per l'autenticazione al padre
	*/
	fork();
	if (getpid() > 0)
	{
		/* Effettua la connessione al server:
    	Server(char* RedisIP, int RedisPort, int serverPort, char* streamIN, char* streamOUT);
    	Cambio il verso degli stream per ovvie ragioni.
		La porta 160 l'ho scelta a caso, vedere se cambiarla
    	*/
	    Server srv(REDIS_IP, REDIS_PORT, 160, WRITE_STREAM, READ_STREAM);


	}
    // Qui sotto tento un sistema di Autenticazione
	std::cout << "Richiesta di autenticazione\n";
	reply = RedisCommand(c2r, "XADD %s * %s %s", WRITE_STREAM, "Mail", "abc@gmail.com");
    assertReplyType(c2r, reply, REDIS_REPLY_STRING);
    freeReplyObject(reply);
}

int main()
{
    ConnectToServer();
    return 0;
}
