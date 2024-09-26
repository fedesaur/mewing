#include "recuperaIndirizzi.h"

std::pair<int, Indirizzo*> recuperaIndirizzi(const char* mail)
{
    std::pair<int, Indirizzo*> risultato;
    char comando[1000];
    int rows;
    int RIGHE;
    Indirizzo* INDIRIZZI;
    redisReply* addressReply;
    redisReply* reply;
    redisContext *c2r;
    PGresult *res;

    // Connessione a Redis
    c2r = redisConnect(REDIS_IP, REDIS_PORT);  // Redis su localhost
    if (c2r == nullptr || c2r->err) {
        std::cerr << "Errore nella connessione a Redis" << std::endl;
        risultato.first = -1;
        risultato.second = nullptr;
        return risultato;
    }


    // recupera dal DB
    Con2DB db(HOSTNAME, DB_PORT, USERNAME_CUST, PASSWORD_CUST, DB_NAME); // Effettua la connessione al database
    try
    {
        sprintf(comando, "SELECT ind.id, ind.via, ind.civico, ind.cap, ind.citta, ind.stato "
        "FROM indirizzo ind, custadd cst, customers cus WHERE cst.customer = cus.id AND cus.mail = '%s' AND cst.addr = ind.id", mail);
        res = db.ExecSQLtuples(comando);
        rows = PQntuples(res);
        if (rows > 0)
        {
            INDIRIZZI = new Indirizzo[rows];

            for (int i = 0; i < rows; i++)
            {
                // Recupera gli attributi degli dalla query sopra svolta...
                int ID = atoi(PQgetvalue(res, i, PQfnumber(res, "id")));
                const char* via = PQgetvalue(res, i, PQfnumber(res, "via"));
                int civico = atoi(PQgetvalue(res, i, PQfnumber(res, "civico")));
                const char* CAP = PQgetvalue(res, i, PQfnumber(res, "cap"));
                const char* citta = PQgetvalue(res, i, PQfnumber(res, "citta"));
                const char* stato = PQgetvalue(res, i, PQfnumber(res, "stato"));

                // e li assegna all'i-esimo Indirizzo in indirizzi
                INDIRIZZI[i].ID = ID;
                INDIRIZZI[i].via = strdup(via);
                INDIRIZZI[i].civico = civico;
                INDIRIZZI[i].CAP = strdup(CAP);
                INDIRIZZI[i].citta = strdup(citta);
                INDIRIZZI[i].stato = strdup(stato);

                // Memorizza il prodotto in Redis come hash
                redisCommand(c2r, "HMSET indirizzo:%d id %d via %s civico %d cap %s città %s stato %s", 
                            ID, ID, via, civico, CAP, citta, stato);

                // Aggiungi l'ID del prodotto alla lista associata all'email
                redisCommand(c2r, "RPUSH indirizzi:%s %d", mail, ID);
            }

            risultato.first = rows;
            risultato.second = INDIRIZZI;
        } else {
            risultato.first = 0;
            risultato.second = nullptr;
        }
        PQclear(res);
    }
    catch (...) 
    {
        risultato.first = -1;
        risultato.second = nullptr;
        PQclear(res);
    }
    
    redisFree(c2r);  // Chiudi connessione Redis
    return risultato;
}
