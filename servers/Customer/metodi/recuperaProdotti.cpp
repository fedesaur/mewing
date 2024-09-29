#include "recuperaProdotti.h"

bool recuperaProdotti(const char* mail)
{
    char comando[1000];
    int rows;
    int RIGHE;
    redisReply* productReply;
    redisReply* reply;
    redisContext *c2r;
    PGresult *res;

    // Connessione a Redis
    c2r = redisConnect(REDIS_IP, REDIS_PORT);  // Redis su localhost
    if (c2r == nullptr || c2r->err) {
        std::cerr << "Errore nella connessione a Redis" << std::endl;
        return false;
    }

    reply = RedisCommand(c2r, "DEL prodottiRic:%s", mail);
    assertReply(c2r, reply);
    freeReplyObject(reply);
    
    Con2DB db(HOSTNAME, DB_PORT, USERNAME_CUST, PASSWORD_CUST, DB_NAME); // Effettua la connessione al database
    try
    {
        sprintf(comando, "SELECT pr.id, pr.descrizione, pr.nome, fr.nome AS nomeF, pr.prezzo FROM prodotto pr, fornitore fr WHERE pr.fornitore = fr.id");
        res = db.ExecSQLtuples(comando);
        int rows = PQntuples(res);
        if (rows > 0)
        {

            for (int i = 0; i < rows; ++i)
            {
            // Recupera gli attributi dei prodotti dalla query sopra svolta...
                int ID = atoi(PQgetvalue(res, i, PQfnumber(res, "id")));
                const char* descrizione = PQgetvalue(res, i, PQfnumber(res, "descrizione"));
                double prezzo = atof(PQgetvalue(res, i, PQfnumber(res, "prezzo")));
                const char* nome = PQgetvalue(res, i, PQfnumber(res, "nome"));
                const char* fornitore = PQgetvalue(res, i, PQfnumber(res, "nomeF"));

                // Memorizza il prodotto in Redis come hash
                redisCommand(c2r, "HMSET prodottoRic:%d id %d descrizione %s prezzo %f nome %s fornitore %s", 
                            ID, ID, descrizione, prezzo, nome, fornitore);

                // Aggiungi l'ID del prodotto alla lista associata all'email
                redisCommand(c2r, "RPUSH prodottiRic:%s %d", mail, ID);
            }
        }
        PQclear(res);
        return true;
    }
    catch (...) 
    {
        PQclear(res);
        redisFree(c2r);  // Chiudi connessione Redis
        return false;
    }
}

/*
std::pair<int, Prodotto*> recuperaProdottiPerNome(int clientSocket, std::string nome)
{
    char buffer[1024] = {0};
    char comando[1000];
    std::pair<int, Prodotto*> risultato;
    PGresult *res;
    Con2DB db(HOSTNAME, DB_PORT, USERNAME_CUST, PASSWORD_CUST, DB_NAME);  // Effettua la connessione al database
    try
    {
        std::string searched = "%" + nome + "%";
        sprintf(comando, "SELECT pr.id, pr.descrizione, pr.nome, pr.prezzo, fr.nome AS nomeF "
        "FROM prodotto pr, fornitore fr WHERE pr.fornitore = fr.id AND pr.nome LIKE '%s' ", searched.c_str());
        res = db.ExecSQLtuples(comando);
        int RIGHE = PQntuples(res);
        if (RIGHE > 0)
        {
            // Prima mostriamo all'utente i prodotti disponibili..
            std::string request = "PRODOTTI DISPONIBILI:\n"; //... e lo stampa
	        send(clientSocket, request.c_str(), request.length(), 0); // Invia il messaggio pre-impostato all'utente
            Prodotto* prodottiDisponibili = new Prodotto[RIGHE];

            for (int i = 0; i < RIGHE; i++)
            {
            // Recupera gli attributi dei prodotti dalla query sopra svolta...
                int ID = atoi(PQgetvalue(res, i, PQfnumber(res, "id")));
                const char* descrizione = PQgetvalue(res, i, PQfnumber(res, "descrizione"));
                double prezzo = atof(PQgetvalue(res, i, PQfnumber(res, "prezzo")));
                const char* nome = PQgetvalue(res, i, PQfnumber(res, "nome"));
                const char* fornitore = PQgetvalue(res, i, PQfnumber(res, "nomeF"));

            // Assegna gli attributi all'i-esimo Prodotto in prodottiDisponibili
                prodottiDisponibili[i].ID = ID;
                prodottiDisponibili[i].descrizione = descrizione;
                prodottiDisponibili[i].prezzo = prezzo;
                prodottiDisponibili[i].nome = nome;
                prodottiDisponibili[i].fornitore = fornitore;
            }
            risultato.first = RIGHE;
            risultato.second = prodottiDisponibili;
        } else {
            risultato.first = 0;
            risultato.second =  nullptr;
        }
    }
    catch(...) // Controlla che la query sia andata a buon fine
    {
        std::string errore = "C'è stato un errore nel database!\n"; // Seleziona la frase del turno
	    send(clientSocket, errore.c_str(), errore.length(), 0); // Invia il messaggio pre-impostato all'utente
        risultato.first = -1;
        risultato.second = nullptr;
    }
    PQclear(res);
    return risultato; 
}
*/
