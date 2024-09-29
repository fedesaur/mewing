#include "recuperaIndirizzi.h"

bool recuperaIndirizzi(const char* mail, int userID)
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

    reply = RedisCommand(c2r, "DEL indirizzi:%s", mail);
    assertReply(c2r, reply);
    freeReplyObject(reply);
    
    Con2DB db(HOSTNAME, DB_PORT, USERNAME_CUST, PASSWORD_CUST, DB_NAME); // Effettua la connessione al database
    try
    {
        sprintf(comando, "SELECT ind.id, ind.via, ind.civico, ind.cap, ind.citta, ind.stato "
        "FROM indirizzo ind, custadd cst WHERE cst.customer = %d AND cst.addr = ind.id", userID);
        res = db.ExecSQLtuples(comando);
        rows = PQntuples(res);
        if (rows > 0)
        {

            for (int i = 0; i < rows; i++)
            {
                // Recupera gli attributi degli indirizzi dalla query sopra svolta
                int ID = atoi(PQgetvalue(res, i, PQfnumber(res, "id")));
                const char* via = PQgetvalue(res, i, PQfnumber(res, "via"));
                int civico = atoi(PQgetvalue(res, i, PQfnumber(res, "civico")));
                const char* CAP = PQgetvalue(res, i, PQfnumber(res, "cap"));
                const char* citta = PQgetvalue(res, i, PQfnumber(res, "citta"));
                const char* stato = PQgetvalue(res, i, PQfnumber(res, "stato"));

                // Memorizza il prodotto in Redis come hash
                redisCommand(c2r, "HMSET indirizzo:%d id %d via %s civico %d cap %s città %s stato %s", 
                            ID, ID, via, civico, CAP, citta, stato);

                // Aggiungi l'ID del prodotto alla lista associata all'email
                redisCommand(c2r, "RPUSH indirizzi:%s %d", mail, ID);
            }

        }
        PQclear(res);
        redisFree(c2r);  // Chiudi connessione Redis
        return true;
    }
    catch (...) 
    {
        PQclear(res);
        redisFree(c2r);  // Chiudi connessione Redis
        return false;

    }
}
