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
    reply = RedisCommand(c2r, "XREVRANGE %s + - COUNT 1", WRITE_STREAM);
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

    std::cout << "Email letta dallo stream: " << received_email << std::endl;
    std::cout.flush();
    const char* mail = received_email.c_str();

	/*
	 	Controlla se esiste un Customer con quella mail; se non esiste, lo crea.
		Se vi sono problemi od errori, ritorna false
	*/
	bool esito = recuperaCustomer(db, clientSocket, mail);
    return esito;
}

bool recuperaCustomer(Con2DB db, int clientSocket, const char* mail)
{
    PGresult *res;
    char comando[1000];
    int rows;
    std::cout << "sto recuperando il customer " << std::endl;
    std::cout.flush();
    
    sprintf(comando, "SELECT * FROM customers WHERE mail = '%s' ", mail);
    res = db.ExecSQLtuples(comando);

    rows = PQntuples(res);
    if (rows > 0) // Se viene trovato un utente con quella mail...
    {
		//...vengono recuperati i suoi dati ed inviati al server tramite Redis
        int ID = atoi(PQgetvalue(res, 0, PQfnumber(res, "id")));
        const char* nome = PQgetvalue(res, 0, PQfnumber(res, "nome"));
        const char* cognome = PQgetvalue(res, 0, PQfnumber(res, "cognome"));
        int abita = atoi(PQgetvalue(res, 0, PQfnumber(res, "abita")));
		bool esito = inviaDati(ID,nome,cognome,mail,abita);
        PQclear(res); // <- Importante metterlo DOPO InviaDati altrimenti i dati vengono cancellati
        return esito;
    }
    // Altrimenti crea un nuovo customer tramite funzione ausiliaria
    PQclear(res);
    return creaCustomer(db, clientSocket, mail);
}

bool creaCustomer(Con2DB db, int clientSocket, const char* mail)
{
	/* 
		Sono richiesti 7 dati all'utente:
		Nome, Cognome, Via, Civico, CAP, Città, Stato
	*/
	PGresult *res;
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
	std::string FRASI[] = {"Inserisci il tuo Nome\n",
	"Inserisci il tuo Cognome\n",
	"Inserisci la Via del tuo indirizzo\n",
	"Inserisci il Civico del tuo indirizzo\n",
	"Inserisci il CAP del tuo indirizzo\n",
	"Inserisci la Città del tuo indirizzo\n",
	"Inserisci lo Stato del tuo indirizzo\n"};

	// Chiede i dati neccessari per creare il customer e l'indirizzo
	while (datiRicevuti < datiRichiesti)
	{
		std::string temp;
		char buffer[1024] = {0};
		std::string request = FRASI[datiRicevuti]; // Seleziona la frase del turno
		send(clientSocket, request.c_str(), request.length(), 0); // Invia il messaggio pre-impostato all'utente
		int bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0); // Riceve la risposta dall'utente e la memorizza nello stream
		if (bytesRead > 0)
		{
			// bool correctInput = true; Nel caso i dati non vadano bene, si potrebbe pensare a ripetere quel passaggio
			std::cout << buffer;
			switch(datiRicevuti)
			{
				case 0:
					nome = buffer;
					nome.pop_back();
					datiRicevuti++;
					break;
				case 1:
					cognome = buffer;
					cognome.pop_back();
					datiRicevuti++;
					break;
				case 2:
					via = buffer;
					via.pop_back();
					datiRicevuti++;
					break;
				case 3:
					temp = buffer;
					temp.pop_back();
					if (isNumber(temp) && stoi(temp) >= 0)
					{
						civico = stoi(temp);
						datiRicevuti++;
					} else {
						std::string errore = "Il civico deve essere un numero positivo"; // Seleziona la frase del turno
						send(clientSocket, errore.c_str(), errore.length(), 0); // Invia il messaggio pre-impostato all'utente
					}
				case 4:
					temp = buffer;
					temp.pop_back();
					if (isNumber(temp) && temp.length() == 5)
					{
						CAP = stoi(temp);
						datiRicevuti++;
					} else {
						std::string errore = "Il CAP deve essere una sequenza di 5 cifre"; // Seleziona la frase del turno
						send(clientSocket, errore.c_str(), errore.length(), 0); // Invia il messaggio pre-impostato all'utente
					}
				case 5:
					city = buffer;
					city.pop_back();
					datiRicevuti++;
					break;
				case 6:
					stato = buffer;
					stato.pop_back();
					datiRicevuti++;
					break;
			}
		}
		else return false;  // Se avviene un errore, l'operazione viene interrotta e non ritorna nulla
	}
	// Viene inserito il nuovo indirizzo nel database
	sprintf(comando, "INSERT INTO Indirizzo(via, civico, cap, citta, stato) VALUES('%s', %d, %d, '%s', '%s') RETURNING id",
	via.c_str(), civico, CAP, city.c_str(), stato.c_str());
	res = db.ExecSQLtuples(comando); 
	if (PQresultStatus(res) != PGRES_TUPLES_OK) return false; // Controlla che la query sia andata a buon fine
	int abita = atoi(PQgetvalue(res, 0, PQfnumber(res, "id"))); // Recupera l'ID dell'indirizzo appena aggiunto

	// Viene inserito il nuovo customer nel database
	sprintf(comando, "INSERT INTO customers(nome, cognome, mail, abita) VALUES('%s', '%s', '%s', %d) RETURNING id",
	nome.c_str(), cognome.c_str(), mail, abita);
	res = db.ExecSQLtuples(comando);
	if (PQresultStatus(res) != PGRES_TUPLES_OK) return false; // Controlla che la query sia andata a buon fine
	int ID = atoi(PQgetvalue(res, 0, PQfnumber(res, "id"))); // Recupera l'ID dell'utente appena creato
	bool esito = inviaDati(ID,nome.c_str(),cognome.c_str(),mail,abita); // Invia i dati tramite Redis
	PQclear(res);	
	return esito;
}

bool inviaDati(int ID, const char* nome, const char* cognome, const char* mail, int abita)
{
	char messaggio[1000];
    redisContext *c2r;
    redisReply *reply;
    c2r = redisConnect(REDIS_IP, REDIS_PORT);
    
	if (c2r == nullptr || c2r->err)
    {
        std::cerr << "Errore nella connessione a Redis: " << (c2r ? c2r->errstr : "Impossibile connettersi a Redis") << std::endl;
        return true;
    }
	// Prepara il comando Redis
	sprintf(messaggio, "XADD %s * ID %d nome %s cognome %s mail %s abita %d", READ_STREAM, ID, nome, cognome, mail, abita);
	reply = RedisCommand(c2r, messaggio); // Invia tutti i campi richiesti al Redis stream
    if (reply == nullptr) {
        std::cerr << "Errore nell'invio del comando XADD: " << c2r->errstr << std::endl;
        redisFree(c2r);
        return false;
    } else if (reply->type != REDIS_REPLY_STRING) {
        std::cerr << "Risposta inattesa da XADD: " << reply->str << std::endl;
        freeReplyObject(reply);
        redisFree(c2r);
        return false;
    }
    std::cout << "Dati inviati con successo." << std::endl;
    freeReplyObject(reply);
    redisFree(c2r);
    return true;
}
