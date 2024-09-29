#include "ricercaOrdini.h"

bool ricercaOrdini(const char* piva)
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

    reply = RedisCommand(c2r, "DEL ordini:%s", piva);
    assertReply(c2r, reply);
    freeReplyObject(reply);
    
    Con2DB db(HOSTNAME, DB_PORT, USERNAME_TRAS, PASSWORD_TRAS, DB_NAME); // Effettua la connessione al database
    try
    {
        // Prima recuperiamo gli ordini NON consegnati
        sprintf(comando, "SELECT ord.id, cst.mail, ord.datarich, ord.stato, ord.pagamento, ind.via, ind.civico, ind.cap, ind.citta, ind.stato AS statoIND, ord.totale "
        "FROM indirizzo ind, customers cst, ordine ord "
        "WHERE ind.id = ord.indirizzo AND ord.customer = cst.id AND ord.id NOT IN (SELECT ordine FROM transord) "
        "AND ord.stato = 'pendente' ORDER BY ord.datarich");
        res = db.ExecSQLtuples(comando);
        rows = PQntuples(res);
        if (rows > 0)
        {
            for (int i = 0; i < rows; i++)
            {
                // Recupera gli attributi degli ordini dalla query sopra svolta...
                int ID = atoi(PQgetvalue(res, i, PQfnumber(res, "id")));
                const char* mail = PQgetvalue(res, i, PQfnumber(res, "mail"));
                unsigned char* data = (unsigned char*) PQgetvalue(res, i, PQfnumber(res, "datarich"));
                time_t time = static_cast<time_t>(std::stoll(reinterpret_cast<char*>(data))); // Converte il timestamp in time_t
                std::string tempo = std::to_string(time); // Converte il tempo in una stringa
                double totale = atof(PQgetvalue(res, i, PQfnumber(res, "totale")));
                const char* pagamento = PQgetvalue(res, i, PQfnumber(res, "pagamento"));

                const char* via = PQgetvalue(res, i, PQfnumber(res, "via"));
                int civico = atoi(PQgetvalue(res, i, PQfnumber(res, "civico")));
                const char* CAP = PQgetvalue(res, i, PQfnumber(res, "cap"));
                const char* city = PQgetvalue(res, i, PQfnumber(res, "citta"));
                const char* stato = PQgetvalue(res, i, PQfnumber(res, "statoIND"));
                

                // Memorizza il prodotto in Redis come hash
                redisCommand(c2r, "HMSET ordine:%d id %d mail %s data %s totale %f pagamento %s via %s civico %d CAP %s city %s stato %s", 
                            ID, ID, mail, tempo.c_str(), totale, pagamento, via, civico, CAP, city, stato);

                // Aggiungi l'ID del prodotto alla lista associata all'email
                redisCommand(c2r, "RPUSH ordini:%s %d", piva, ID);
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

       
            
