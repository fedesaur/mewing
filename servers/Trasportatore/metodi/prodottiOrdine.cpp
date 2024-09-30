#include "prodottiOrdine.h"

bool prodottiOrdine(int orderID)
{
    char comando[1000];
    int rows;
    redisReply* reply;
    redisContext *c2r;
    PGresult *res;

    // Connessione a Redis
    c2r = redisConnect(REDIS_IP, REDIS_PORT);  // Redis su localhost
    if (c2r == nullptr || c2r->err) {
        std::cerr << "Errore nella connessione a Redis" << std::endl;
        return false;
    }
    reply = RedisCommand(c2r, "DEL prodottiOrdine:%d", orderID);
    assertReply(c2r, reply);
    freeReplyObject(reply);

    Con2DB db(HOSTNAME, DB_PORT, USERNAME_TRAS, PASSWORD_TRAS, DB_NAME); // Effettua la connessione al database
    try
    {
        sprintf(comando, "SELECT pr.id, pr.descrizione, pr.nome, pr.prezzo, fr.nome AS nomeF, pn.quantita "
            "FROM prodotto pr, prodinord pn, fornitore fr WHERE pn.prodotto = pr.id "
            "AND pr.fornitore = fr.id AND pn.ordine = %d", orderID);
        res = db.ExecSQLtuples(comando);
        rows = PQntuples(res); // Si presume ci siano prodotti nell'ordine
        for (int j = 0; j < rows; j++)
        {
            int IDProd = atoi(PQgetvalue(res, j, PQfnumber(res, "id")));
            const char* descrizione = PQgetvalue(res, j, PQfnumber(res, "descrizione"));
            double prezzo = atof(PQgetvalue(res, j, PQfnumber(res, "prezzo")));
            const char* nome = PQgetvalue(res, j, PQfnumber(res, "nome"));
            const char* fornitore = PQgetvalue(res, j, PQfnumber(res, "nomeF"));
            int quantita = atoi(PQgetvalue(res, j, PQfnumber(res, "quantita")));

            redisCommand(c2r, "HMSET prodotto:%d id %d nome %s descrizione %s fornitore %s prezzo %f quantita %d", IDProd, IDProd, nome, descrizione, fornitore, prezzo, quantita);

            // Aggiungi l'ID del prodotto alla lista associata all'ID dell'ordine
            redisCommand(c2r, "RPUSH prodottiOrdine:%d %d", orderID, IDProd);
        }
        PQclear(res);
        redisFree(c2r);
        return true;
    }
    catch (...) 
    {
        PQclear(res);
        redisFree(c2r);
        return false;
    }
}
