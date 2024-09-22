#include "autenticazione.h"

int autentica(const char* mail)
{
	redisContext *c2r;
	redisReply* reply;
	PGresult *res;
    char comando[1000];
    int rows;
	int ID;
	Con2DB db(HOSTNAME, DB_PORT, USERNAME, PASSWORD, DB_NAME); // Effettua la connessione al database
	
	// Effettua la connessione a Redis
	c2r = redisConnect(REDIS_IP, REDIS_PORT);
    if (c2r == nullptr || c2r->err) {
        response.send(Pistache::Http::Code::Internal_Server_Error, "Unable to connect to Redis");
        return;
    }
	
	try
	{
		// Recupera il fornitore tramite l'email
		sprintf(comando, "SELECT id FROM fornitore WHERE mail = '%s' ", mail);
    	res = db.ExecSQLtuples(comando);

    	rows = PQntuples(res);
		ID = 0;
    	if (rows > 0) // Se viene trovato un utente con quella mail...
    	{
			//...vengono recuperati i suoi dati ed inviati al server tramite Redis
        	int ID = atoi(PQgetvalue(res, 0, PQfnumber(res, "id")));
			reply = RedisCommand(c2r, "XADD %s * %s %d", WRITE_STREAM, email, ID);
        	assertReplyType(c2r, reply, REDIS_REPLY_STRING);
        	freeReplyObject(reply);
    	}
	}
	catch(...)
	{
		ID = -1;
	}
	PQclear(res);
	return ID;
}

bool creaSupplier(Con2DB db, int clientSocket, const char* mail)
{
	/* 
		Sono richiesti 8 dati all'utente:
		Nome, IVA, Telefono, Via, Civico, CAP, Città, Stato
	*/
	PGresult *res;
	int datiRichiesti = 8;
	int datiRicevuti = 0;
	char comando[1000];

	std::string nome;
	std::string IVA;
	std::string telefono;
	std::string via;
	int civico;
	int CAP;
	std::string city;
	std::string stato;
	
	// Di seguito, le frasi mostrate all'utente ad ogni fase della creazione del Customer
	std::string FRASI[] = {"Inserisci il tuo Nome\n",
	"Inserisci la tua partita IVA\n",
	"Inserisci il tuo numero di telefono\n",
	"Inserisci la via della sede dell'azienda\n",
	"Inserisci il Civico dell'azienda\n",
	"Inserisci il CAP dell'azienda\n",
	"Inserisci la Città dell'azienda\n",
	"Inserisci lo Stato dell'azienda\n"};

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
			switch(datiRicevuti)
			{
				case 0:
					nome = buffer;
					nome.pop_back();
					datiRicevuti++;
					break;
				case 1:
					temp = buffer;
					temp.pop_back();
					if (temp.length() == 11 && isNumber(temp))
					{
						IVA = temp;
						datiRicevuti++;
					} else {
						std::string errore = "La partita IVA deve essere una stringa numerica di 11 cifre\n"; // Seleziona la frase del turno
						send(clientSocket, errore.c_str(), errore.length(), 0); // Invia il messaggio pre-impostato all'utente
					}
					break;
				case 2:
					temp = buffer;
					temp.pop_back();
					if (temp.length() > 15 || temp.length() < 10 || !isNumber(temp)) 
					{
						std::string errore = "Il numero di telefono deve essere una sequenza di massimo 15 cifre e minimo 10\n";
						send(clientSocket, errore.c_str(), errore.length(), 0);
					} else {
						telefono = temp;
						datiRicevuti++;
					}
					break;
				case 3:
					via = buffer;
					via.pop_back();
					datiRicevuti++;
					break;
				case 4:
					temp = buffer;
					temp.pop_back();
					if (isNumber(temp) && stoi(temp) >= 0)
					{
						civico = stoi(temp);
						datiRicevuti++;
					} else {
						std::string errore = "Il civico deve essere un numero positivo\n"; // Seleziona la frase del turno
						send(clientSocket, errore.c_str(), errore.length(), 0); // Invia il messaggio pre-impostato all'utente
					}
					break;
				case 5:
					temp = buffer;
					temp.pop_back();
					if (isNumber(temp) && temp.length() == 5)
					{
						CAP = stoi(temp);
						datiRicevuti++;
					} else {
						std::string errore = "Il CAP deve essere una sequenza di 5 cifre\n"; // Seleziona la frase del turno
						send(clientSocket, errore.c_str(), errore.length(), 0); // Invia il messaggio pre-impostato all'utente
					}
					break;
				case 6:
					city = buffer;
					city.pop_back();
					datiRicevuti++;
					break;
				case 7:
					stato = buffer;
					stato.pop_back();
					datiRicevuti++;
					break;
			}
		}
	}
	// Viene inserito il nuovo indirizzo nel database
	try
	{
		sprintf(comando, "INSERT INTO Indirizzo(via, civico, cap, citta, stato) VALUES('%s', %d, %d, '%s', '%s') RETURNING id", via.c_str(), civico, CAP, city.c_str(), stato.c_str());
		res = db.ExecSQLtuples(comando); 
		int sede = atoi(PQgetvalue(res, 0, PQfnumber(res, "id"))); // Recupera l'ID dell'indirizzo appena aggiunto
		
		sprintf(comando, "INSERT INTO fornitore(nome, piva, mail, telefono, sede) VALUES('%s', '%s', '%s','%s', %d) RETURNING id",
		nome.c_str(), IVA.c_str(), mail, telefono.c_str(), sede);
		res = db.ExecSQLtuples(comando); // Viene inserito il nuovo customer nel database
		int ID = atoi(PQgetvalue(res, 0, PQfnumber(res, "id"))); // Recupera l'ID dell'utente appena creato
		bool esito = inviaDati(ID,nome.c_str(),IVA.c_str(), telefono.c_str(), sede); // Invia i dati tramite Redis
		PQclear(res);	
		return esito;
	}
	catch(...)
	{
		std::string errore = "C'è stato un problema con le query\n"; // Seleziona la frase del turno
		send(clientSocket, errore.c_str(), errore.length(), 0); // Invia il messaggio pre-impostato all'utente
		return false;
	}
}

bool inviaDati(int ID, const char* nome, const char* piva, const char* telefono, int sede)
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
	sprintf(messaggio, "XADD %s * ID %d nome %s piva %s telefono %s sede %d", READ_STREAM, ID, nome, piva, telefono, sede);
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
