#include "autenticazione.h"

bool autentica()
{
	redisContext *c2r; // c2r contiene le info sul contesto
	redisReply *reply; // reply contiene le risposte da Redis

	c2r = redisConnect(REDIS_IP, REDIS_PORT); // Effettua la connessione a Redis
	// Legge l'ultima partita IVA dallo stream
    reply = RedisCommand(c2r, "XREVRANGE %s + - COUNT 1", WRITE_STREAM);
    if (reply == nullptr || reply->type != REDIS_REPLY_ARRAY || reply->elements == 0)
	{
       std::cerr << "Errore nel comando Redis o stream vuoto" << std::endl;
       return false;
    }

    redisReply* stream = reply -> element[0];
    redisReply* entryFields = stream -> element[1];
    std::string fieldName = entryFields->element[0]->str; // Chiave
    std::string received_piva = entryFields->element[1]->str; // Valore
    freeReplyObject(reply);

    if (received_piva.empty())
      {
	  std::cerr << "Errore: non è stata trovata nessuna p.iva con la chiave specificata." << std::endl;
	  return false; 
      }

    const char* piva = received_piva.c_str();
	/*
	 	Controlla se esiste un Customer con quella mail; se non esiste, lo crea.
		Se vi sono problemi od errori, ritorna false
	*/
	bool esito = recuperaTrasportatore(piva);
    return esito;
}

bool recuperaTrasportatore(const char* piva)
{
    PGresult *res;
    char comando[1000];
    int rows;
	Con2DB db(HOSTNAME, DB_PORT, USERNAME, PASSWORD, DB_NAME); // Effettua la connessione al database
    
	try
	{
		sprintf(comando, "SELECT * FROM trasportatore WHERE piva = '%s' ", piva);
    	res = db.ExecSQLtuples(comando);
		rows = PQntuples(res);
    	if (rows > 0) // Se viene trovato un utente con quella mail...
    	{
			//...vengono recuperati i suoi dati ed inviati al server tramite Redis
        	int ID = atoi(PQgetvalue(res, 0, PQfnumber(res, "id")));
        	const char* nome = PQgetvalue(res, 0, PQfnumber(res, "nome"));
        	const char* piva = PQgetvalue(res, 0, PQfnumber(res, "piva"));
        	int sede = atoi(PQgetvalue(res, 0, PQfnumber(res, "indirizzo")));
			bool esito = inviaDati(ID,nome,piva,sede);
        	PQclear(res); // <- Importante metterlo DOPO InviaDati altrimenti i dati vengono cancellati
        	return esito;
    	}
    	// Altrimenti crea un nuovo customer tramite funzione ausiliaria
    	PQclear(res);
    	return creaTrasportatore(piva);
	}
	catch(...)
	{
		// C'è stato un problema con la query
		PQclear(res);
        return false;
	}
	
    
}

bool creaTrasportatore(const char* piva) 
{
	/* 
		Sono richiesti 6 dati all'utente:
		Nome, Via, Civico, CAP, Città, Stato
	*/
	PGresult *res;
	int datiRichiesti = 6;
	int datiRicevuti = 0;
	char comando[1000];
	Con2DB db(HOSTNAME, DB_PORT, USERNAME, PASSWORD, DB_NAME); // Effettua la connessione al database

	std::string nome;
	std::string via;
	int civico;
	std::string CAP;
	std::string city;
	std::string stato;
	
	// Di seguito, le frasi mostrate all'utente ad ogni fase della creazione del Customer
	std::string FRASI[] = {"Inserisci il Nome dell'azienda\n",
	"Inserisci la Via della sede dell'azienda\n",
	"Inserisci il Civico della sede dell'aziendao\n",
	"Inserisci il CAP della sede dell'azienda\n",
	"Inserisci la Città della sede dell'azienda\n",
	"Inserisci lo Stato della sede dell'azienda\n"};

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
					temp = buffer;
					temp.pop_back();
					if (temp.length() > 100 || temp.length() == 0)
					{
						std::string errore = "Il nome può avere massimo 100 caratteri\n"; // Seleziona la frase del turno
						send(clientSocket, errore.c_str(), errore.length(), 0); // Invia il messaggio pre-impostato all'utente
					} else {
						nome = temp;
						datiRicevuti++;
					}
					break;
				case 1:
					temp = buffer;
					temp.pop_back();
					if (temp.length() > 100 || temp.length() == 0)
					{
						std::string errore = "La via può avere massimo 100 caratteri\n"; // Seleziona la frase del turno
						send(clientSocket, errore.c_str(), errore.length(), 0); // Invia il messaggio pre-impostato all'utente
					} else {
						via = temp;
						datiRicevuti++;
					}
					break;
				case 2:
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
				case 3:
					temp = buffer;
					temp.pop_back();
					if (isNumber(temp) && temp.length() == 5)
					{
						CAP = temp;
						datiRicevuti++;
					} else {
						std::string errore = "Il CAP deve essere una sequenza di 5 cifre\n"; // Seleziona la frase del turno
						send(clientSocket, errore.c_str(), errore.length(), 0); // Invia il messaggio pre-impostato all'utente
					}
					break;
				case 4:
					temp = buffer;
					temp.pop_back();
                    if (temp.length() > 30 || temp.length() == 0)
                    {
                       	std::string errore = "La città deve avere al massimo 30 caratteri\n"; // Seleziona la frase del turno
						send(clientSocket, errore.c_str(), errore.length(), 0); // Invia il messaggio pre-impostato all'utente 
                    } else {
                        city = temp;
                        datiRicevuti++;
                    }
					break;
				case 5:
					temp = buffer;
					temp.pop_back();
                    if (temp.length() > 30 || temp.length() == 0)
                    {
                       	std::string errore = "Lo stato deve avere al massimo 30 caratteri\n"; // Seleziona la frase del turno
						send(clientSocket, errore.c_str(), errore.length(), 0); // Invia il messaggio pre-impostato all'utente 
                    } else {
                        stato = temp;
                        datiRicevuti++;
                    }
					break;
			}
        } else {
            std::string errore = "Input non valido, riprova.\n";
            send(clientSocket, errore.c_str(), errore.length(), 0);
        }  // Se avviene un errore, l'operazione viene interrotta e non ritorna nulla
	}
	try
    {
	    sprintf(comando, "INSERT INTO Indirizzo(via, civico, cap, citta, stato) VALUES('%s', %d, '%s', '%s', '%s') RETURNING id", via.c_str(), civico, CAP.c_str(), city.c_str(), stato.c_str());
	    res = db.ExecSQLtuples(comando);
        int address = atoi(PQgetvalue(res, 0, PQfnumber(res, "id"))); // Recupera l'ID dell'indirizzo appena aggiunto
        sprintf(comando, "INSERT INTO trasportatore(piva, nome, indirizzo) VALUES ('%s', '%s', %d) RETURNING id", 
		piva, nome.c_str(), address);
	    PQclear(res);
		res = db.ExecSQLtuples(comando);
        std::string errore = "Trasportatore aggiunto al database\n\n";
		send(clientSocket, errore.c_str(), errore.length(), 0); // Invia il messaggio pre-impostato all'utente    
        int trasporterID = atoi(PQgetvalue(res, 0, PQfnumber(res, "id"))); // Recupera l'ID del trasportatore appena aggiunto
		inviaDati(trasporterID, nome.c_str(), piva, address);
		PQclear(res);
		return true;
    }
    catch(...) //Se c'è stato un errore, lo segnala all'utente
    {
        std::string errore = "C'è stato un errore nel database\n\n";
		send(clientSocket, errore.c_str(), errore.length(), 0); // Invia il messaggio pre-impostato all'utente    
        PQclear(res);
        return false;
    }
}

bool inviaDati(int ID, const char* nome, const char* piva, int sede)
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
	sprintf(messaggio, "XADD %s * ID %d nome %s piva %s sede %d", READ_STREAM, ID, nome, piva, sede);
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
