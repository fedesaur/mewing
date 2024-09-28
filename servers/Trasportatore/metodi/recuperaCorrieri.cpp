#include "recuperaCorrieri.h"

bool recuperaCorrieri(const char* IVA, int trasporterID)
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
    reply = RedisCommand(c2r, "DEL corrieri:%s", IVA);
    assertReply(c2r, reply);
    freeReplyObject(reply);

    // Recupera dal DB
    Con2DB db(HOSTNAME, DB_PORT, USERNAME_TRAS, PASSWORD_TRAS, DB_NAME); // Effettua la connessione al database
    try
    {
        // Prima recuperiamo gli ordini NON consegnati
        sprintf(comando, "SELECT id, nome, cognome FROM corriere WHERE azienda = %d", trasporterID);        
        res = db.ExecSQLtuples(comando);
        rows = PQntuples(res);
        if (rows > 0)
        {
            for (int i = 0; i < rows; i++)
            {
                int ID = atoi(PQgetvalue(res, i, PQfnumber(res, "id")));
                const char* nome = PQgetvalue(res, i, PQfnumber(res, "nome"));
                const char* cognome = PQgetvalue(res, i, PQfnumber(res, "cognome"));

                // Memorizza il corriere in Redis come hash
                redisCommand(c2r, "HMSET corriere:%d id %d nome %s cognome %s", ID, ID, nome, cognome);

                // Aggiungi l'ID del corriere alla lista associata all'IVA
                redisCommand(c2r, "RPUSH corrieri:%s %d", IVA, ID);
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
