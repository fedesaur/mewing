#include "ricercaProdotti.h"
/*
    CREATE TABLE IF NOT EXISTS prodotto
    (
    id integer NOT NULL DEFAULT nextval('prodotto_id_seq'::regclass),
    descrizione text COLLATE pg_catalog."default" NOT NULL,
    prezzo double precision NOT NULL,
    fornitore integer NOT NULL,
    nome character varying(100) COLLATE pg_catalog."default" NOT NULL,
    CONSTRAINT prodotto_pkey PRIMARY KEY (id),
    CONSTRAINT prodotto_fornitore_fkey FOREIGN KEY (fornitore)
        REFERENCES fornitore (id) MATCH SIMPLE
        ON UPDATE NO ACTION
        ON DELETE NO ACTION
    );
*/
bool cercaProdottiDisponibili(int clientSocket)
{
    int USER_ID;
    PGresult *res;
    redisContext *c2r; // c2r contiene le info sul contesto
	redisReply *reply; // reply contiene le risposte da Redis

	c2r = redisConnect(REDIS_IP, REDIS_PORT); // Effettua la connessione a Redis
	Con2DB db(HOSTNAME, DB_PORT, USERNAME, PASSWORD, DB_NAME); // Effettua la connessione al database

    reply = RedisCommand(c2r, "XREVRANGE %s + - COUNT 1", READ_STREAM);
    if (reply == nullptr || reply->type != REDIS_REPLY_ARRAY || reply->elements == 0)
    {
        std::cerr << "Errore nel comando Redis o stream vuoto" << std::endl;
    }

    std::string id = reply->element[0]->element[1]->element[1]->str; 
    USER_ID = atoi(id.c_str()); // ID Customer
    std::cout << USER_ID << std::endl;

    // Recupera le informazioni su tutti i prodotti disponibili 
    std::pair<int, Prodotto*> risultato = recuperaProdottiDisponibili(db, res, clientSocket);
    if (risultato.first == -1) return false; // Se vi sono errori o non vi sono prodotti
    //...recuperati i prodotti, permette operazioni con quelli trovati e quelli anche nel carrello  
    int righe = risultato.first;
    Prodotto* prodottiDisponibili = risultato.second;
    //for (int i = 0; i < righe; i++) std::cout << prodottiDisponibili[i].ID << prodottiDisponibili[i].descrizione << prodottiDisponibili[i].prezzo << prodottiDisponibili[i].nome <<  prodottiDisponibili[i].fornitore << std::endl;
    
    delete[] risultato.second;
    return true;
}

std::pair<int, Prodotto*>  recuperaProdottiDisponibili(Con2DB db, PGresult *res, int clientSocket)
{
    std::pair <int, Prodotto*> risultato;
    int rows;
    char comando[1000];
    sprintf(comando, "SELECT pr.id, pr.descrizione, pr.nome, pr.prezzo, fr.nome AS nomeF FROM prodotto pr, fornitore fr WHERE pr.fornitore = fr.id");
    res = db.ExecSQLtuples(comando);
    if (PQresultStatus(res) != PGRES_TUPLES_OK)
    {
        risultato.first = -1;
        risultato.second = nullptr;
        return risultato; // Controlla che la query sia andata a buon fine
    }
    rows = PQntuples(res);
    if (rows > 0)
    {
        // Prima mostriamo all'utente i prodotti disponibili..
        Prodotto* prodottiDisponibili = new Prodotto[rows];

        for (int i = 0; i < rows; i++)
        {
            int ID = atoi(PQgetvalue(res, i, PQfnumber(res, "id")));
            const char* descrizione = PQgetvalue(res, i, PQfnumber(res, "descrizione"));
            double prezzo = atof(PQgetvalue(res, i, PQfnumber(res, "prezzo")));
            const char* nome = PQgetvalue(res, i, PQfnumber(res, "nome"));
            const char* fornitore = PQgetvalue(res, i, PQfnumber(res, "nomeF"));
            std::string prodotto = "ID Prodotto: " + std::to_string(ID) + " Nome Prodotto: " + nome + " Descrizione: " + descrizione + " Fornitore: " + fornitore + " Prezzo Prodotto: " + std::to_string(prezzo) + "\n";
	        send(clientSocket, prodotto.c_str(), prodotto.length(), 0);
            
            prodottiDisponibili[i].ID = ID;
            prodottiDisponibili[i].descrizione = descrizione;
            prodottiDisponibili[i].prezzo = prezzo;
            prodottiDisponibili[i].nome = nome;
            prodottiDisponibili[i].fornitore = fornitore;

            std::cout << prodottiDisponibili[i].ID << prodottiDisponibili[i].descrizione << prodottiDisponibili[i].prezzo << prodottiDisponibili[i].nome <<  prodottiDisponibili[i].fornitore << std::endl;
        }
        risultato.first = rows;
        risultato.second = prodottiDisponibili;
        PQclear(res);
        return risultato;
    }
    // Se non ci sono oggetti
	std::string request = "Nessun prodotto disponibile!\n"; // Seleziona la frase del turno
	send(clientSocket, request.c_str(), request.length(), 0); // Invia il messaggio pre-impostato all'utente
    risultato.first = -1;
    risultato.second = nullptr;
    return risultato;
}
