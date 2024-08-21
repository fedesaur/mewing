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

int main()
{
	/*
		Crea un server che si prepara ad avviare una
		connessione con l'utente
	 */
	Server server(REDIS_IP, REDIS_PORT, SERVER_PORT, WRITE_STREAM, READ_STREAM);
	std::cout << "Connessione al server effettuata!" << std::endl;
    return 0;
}
