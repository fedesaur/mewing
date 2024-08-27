#include "autenticazione.h"

bool autentica(int clientSocket)
{
	redisContext *c2r; // c2r contiene le info sul contesto
	redisReply *reply; // reply contiene le risposte da Redis

	c2r = redisConnect(REDIS_IP, REDIS_PORT); // Effettua la connessione a Redis
	Con2DB db(HOSTNAME, DB_PORT, USERNAME, PASSWORD, DB_NAME); // Effettua la connessione al database
	/*
  	 Con2DB(const char *hostname,
	 const char *port,
	 const char *username,
	 const char *password,
	 const char *dbname);
	*/

	// Legge l'ultima mail nello stream
    reply = RedisCommand(c2r, "XREVRANGE %s + - COUNT 1", CUSTOMER_STREAM);
    if (reply == nullptr || reply->type != REDIS_REPLY_ARRAY || reply->elements == 0)
	{
       std::cerr << "Errore nel comando Redis o stream vuoto" << std::endl;
       return false;
    }

    redisReply* stream = reply -> element[0];
    redisReply* entryFields = stream -> element[1];
    std::string fieldName = entryFields->element[0]->str; // Chiave
   	std::string received_email = entryFields->element[1]->str; // Valore
	freeReplyObject(reply);

	if (received_email.empty())
	{
		std::cerr << "Errore: non è stata trovata nessuna email con la chiave specificata." << std::endl;
		return false;
	}
	received_email.pop_back();
	std::cout << "Email letta dallo stream: " << received_email << std::endl;
	const char* mail = received_email.c_str();

	// Controlla se esiste un Customer con quella mail; se non esiste, lo crea
	std::tuple <int, const char*, const char*, const char*, int> cust = recuperaCustomer(db, clientSocket, mail); 
	if (get<0>(cust) == -1) return false; // Se c'è stato un errore, ritorna false, ma bisogna rivedere come mostrare errore

	reply = RedisCommand(c2r, "XADD %s * %s %s", CUSTOMER_STREAM, "CustomerID", get<0>(cust));
	assertReplyType(c2r, reply, REDIS_REPLY_STRING);
    freeReplyObject(reply);

	reply = RedisCommand(c2r, "XADD %s * %s %s", CUSTOMER_STREAM, "CustomerName", get<1>(cust));
	assertReplyType(c2r, reply, REDIS_REPLY_STRING);
    freeReplyObject(reply);

	reply = RedisCommand(c2r, "XADD %s * %s %s", CUSTOMER_STREAM, "CustomerSurname", get<2>(cust));
	assertReplyType(c2r, reply, REDIS_REPLY_STRING);
    freeReplyObject(reply);

	reply = RedisCommand(c2r, "XADD %s * %s %s", CUSTOMER_STREAM, "CustomerMail", mail);
	assertReplyType(c2r, reply, REDIS_REPLY_STRING);
    freeReplyObject(reply);

	reply = RedisCommand(c2r, "XADD %s * %s %s", CUSTOMER_STREAM, "CustomerAddress", get<4>(cust));
	assertReplyType(c2r, reply, REDIS_REPLY_STRING);
    freeReplyObject(reply);
    return true;
}

std::tuple <int, const char*, const char*, const char*, int> recuperaCustomer(Con2DB db, int clientSocket, const char* mail)
{
	PGresult *res;
	char comando[1000];
	int rows;


	// sprintf si occupa di creare una stringa con una data formattazione
	sprintf(comando, "SELECT * FROM customers WHERE mail = '%s' ", mail);

	res = db.ExecSQLtuples(comando); //Esegue la query sopra citata

	rows = PQntuples(res); // Numero delle righe della query
	if (rows > 0) // Se è stato trovato un utente con quella mail...
	{
		int ID = atoi(PQgetvalue(res, 0, PQfnumber(res, "id")));
		const char* nome = PQgetvalue(res, 0, PQfnumber(res, "nome"));
		const char* cognome = PQgetvalue(res, 0, PQfnumber(res, "cognome"));
		int abita = atoi(PQgetvalue(res, 0, PQfnumber(res, "abita")));
		std::tuple <int, const char*, const char*, const char*, int> cust(ID, nome, cognome, mail, abita);
		PQclear(res);
		return cust;
	}
	// ...altrimenti crealo tramite funzione ausiliaria
	PQclear(res);
	return creaCustomer(db, clientSocket, mail);
}

std::tuple <int, const char*, const char*, const char*, int> creaCustomer(Con2DB db, int clientSocket, const char* mail)
{
	/* 
		Sono richiesti 7 dati all'utente:
		Nome, Cognome, Via, Civico, CAP, Città, Stato
	*/
	int datiRichiesti = 7;
	int datiRicevuti = 0;
	char comando[1000];

	std::string nome;
	std::string cognome;
	std::string via;
	int civico;
	int CAP;
	std::string city;
	std::string stato;

	// Di seguito, le frasi mostrate all'utente ad ogni fase della creazione del Customer
	std::string frasi[7] = {"Inserisci il tuo Nome\n",
	"Inserisci il tuo Cognome\n",
	"Inserisci la Via del tuo indirizzo\n",
	"Inserisci il Civico del tuo indirizzo\n",
	"Inserisci il CAP del tuo indirizzo\n",
	"Inserisci la Città del tuo indirizzo\n",
	"Inserisci lo Stato del tuo indirizzo\n"};
	
	// Chiede i dati neccessari per creare il customer e l'indirizzo
	while (datiRicevuti < datiRichiesti)
	{
		char buffer[1024] = {0};
		std::string request = frasi[datiRicevuti];
		send(clientSocket, request.c_str(), request.length(), 0);
		// Invia il messaggio pre-impostato all'utente

		int bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
		if (bytesRead > 0)
		{
			switch(datiRicevuti)
			{
				case 0:
					nome = buffer.pop_back();
					break;
				case 1:
					cognome = buffer.pop_back();
					break;
				case 2:
					via = buffer.pop_back();
					break;
				case 3:
					std::string civ = buffer.pop_back();
					civico = atoi(civ);
					break;
				case 4:
					std::string cap = buffer.pop_back();
					CAP = atoi(cap);
					break;
				case 5:
					city = buffer.pop_back();
					break;
				case 6:
					stato = buffer.pop_back();
					break;
			}
			datiRicevuti++; // Nel caso i dati non vadano bene, si potrebbe pensare a ripetere quel passaggio
		}
		else
		{
			std::tuple <int, const char*, const char*, const char*, int> failed(-1, "", "", "", -1);
			return failed; 
		}; // Se avviene un errore, l'operazione viene interrotta e non ritorna nulla
	}

	sprintf(comando, "INSERT INTO Indirizzo(via, civico, cap, citta, stato) VALUES('%s', %d, %d, '%s', '%s') RETURNING id",
	via.c_str(), civico, cap, city.c_str(), stato.c_str());
	res = db.ExecSQLtuples(comando); // Inserisci l'indirizzo nel database e ne ritorna l'ID
	if (PQresultStatus(res) != PGRES_TUPLES_OK)
	{
		std::tuple <int, const char*, const char*, const char*, int> failed(-1, "", "", "", -1);
		return failed; // Controlla che la query sia andata a buon fine
	}
	int abita = atoi(PQgetvalue(res, 0, PQfnumber(res, "id")));
	PQclear(res);


	sprintf(comando, "INSERT INTO Customer(nome, cognome, mail, abita) VALUES('%s', '%s', '%s', %d) RETURNING id",
	nome.c_str(), cognome.c_str(), mail, abita);
	res = db.ExecSQLtuples(comando); // Inserisce il customer nel database e ne ritorna l'ID
	if (PQresultStatus(res) != PGRES_TUPLES_OK)
	{
		std::tuple <int, const char*, const char*, const char*, int> failed(-1, "", "", "", -1);
		return failed; 
	}; // Controlla che la query sia andata a buon fine
	int ID = atoi(PQgetvalue(res, 0, PQfnumber(res, "id")));
	PQclear(res);

	std::tuple <int, const char*, const char*, const char*, int> cust(ID, nome.c_str(), cognome.c_str(), mail, abita);
	return cust; // Ritorna il customer appena creato
}
