#include "recuperaForniti.h"

bool recuperaForniti(const char* mail)
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

    reply = RedisCommand(c2r, "DEL prodottiForniti:%s", mail);
    assertReply(c2r, reply);
    freeReplyObject(reply);
    
    Con2DB db(HOSTNAME, DB_PORT, USERNAMEP, PASSWORDP, DB_NAME);  // Connessione al DB
    try
    {
        // Prima recuperiamo gli ordini NON consegnati
        sprintf(comando, "SELECT pr.id, pr.descrizione, pr.prezzo, pr.nome FROM prodotto pr, fornitore fr WHERE pr.fornitore = fr.id AND fr.mail = '%s' ", mail);
        res = db.ExecSQLtuples(comando);
        rows = PQntuples(res);
        if (rows > 0)
        {
            for (int i = 0; i < rows; i++)
            {
                int ID = atoi(PQgetvalue(res, i, PQfnumber(res, "id")));
                const char* descrizione = PQgetvalue(res, i, PQfnumber(res, "descrizione"));
                double prezzo = atof(PQgetvalue(res, i, PQfnumber(res, "prezzo")));
                const char* nome = PQgetvalue(res, i, PQfnumber(res, "nome"));

                // Memorizza il prodotto in Redis come hash
                redisCommand(redis, "HMSET prodottoFornito:%d id %d nome %s descrizione %s prezzo %f", ID, ID, nome, descrizione, prezzo);

                // Aggiungi l'ID del prodotto alla lista associata all'email
                redisCommand(redis, "RPUSH prodottiForniti:%s %d", mail, ID);
            }

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

       
            
