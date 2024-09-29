#include "recuperaCarrello.h"

bool recuperaCarrello(int id, const char* mail)
{
    char comando[1000];
    int rows;
    int RIGHE;
    redisReply* productReply = nullptr;
    redisReply* reply = nullptr;
    redisContext *c2r = nullptr;
    PGresult *res = nullptr;

    // Connessione a Redis
    c2r = redisConnect(REDIS_IP, REDIS_PORT);  // Redis su localhost
    if (c2r == nullptr || c2r->err) {
        std::cerr << "Errore nella connessione a Redis: " << (c2r ? c2r->errstr : "null") << std::endl;
        if (c2r) redisFree(c2r);  // Chiudi connessione Redis in caso di errore
        return false;
    }

    reply = RedisCommand(c2r, "DEL carrello:%s", mail);
    assertReply(c2r, reply);
    freeReplyObject(reply);
  
    //recupera dal DB e li immette nello stream
    Con2DB db(HOSTNAME, DB_PORT, USERNAME_CUST, PASSWORD_CUST, DB_NAME); // Effettua la connessione al database
    try
    {
        sprintf(comando, "SELECT pr.id, pr.descrizione, pr.nome, pr.prezzo, fr. nome AS nomeF, cr.totale, pc.quantita FROM prodotto pr, carrello cr, prodincart pc, fornitore fr WHERE pc.prodotto = pr.id AND pc.carrello=cr.customer AND pr.fornitore=fr.id AND cr.customer=%d", id);
        res = db.ExecSQLtuples(comando);
        rows = PQntuples(res);
        if (rows > 0)
        {
            for (int i = 0; i < rows; i++)
            {
                // Recupera gli attributi degli dalla query sopra svolta...
                int ID = atoi(PQgetvalue(res, i, PQfnumber(res, "id")));
                const char* descrizione = PQgetvalue(res, i, PQfnumber(res, "descrizione"));
                double prezzo = atof(PQgetvalue(res, i, PQfnumber(res, "prezzo")));
                const char* nome = PQgetvalue(res, i, PQfnumber(res, "nome"));
                const char* fornitore = PQgetvalue(res, i, PQfnumber(res, "nomeF"));
                int quantita = atoi(PQgetvalue(res, i, PQfnumber(res, "quantita")));

                // Memorizza il prodotto in Redis come hash
                redisCommand(c2r, "HMSET prodottoCarr:%d id %d descrizione %s prezzo %f nome %s fornitore %s quantità %d", 
                            ID, ID, descrizione, prezzo, nome, fornitore, quantita);

                // Aggiungi l'ID del prodotto alla lista associata all'email
                redisCommand(c2r, "RPUSH carrello:%s %d", mail, ID);
            }
        }
        PQclear(res);
        redisFree(c2r);  // Chiudi connessione Redis
        return true;
    }
    catch (...) 
    {
        if (res) PQclear(res);
        redisFree(c2r);  // Chiudi connessione Redis
        std::cerr << "Errore durante il recupero dal DB o durante la memorizzazione in Redis" << std::endl;
        return false;
    }
}
