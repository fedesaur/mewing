#include "recuperaOrdini.h"

bool recuperaOrdini(const char* mail)
{
    char comando[1000];
    int rows;
    int RIGHE;
    redisReply* orderReply;
    redisReply* reply;
    redisContext *c2r;
    PGresult *res;

    // Connessione a Redis
    c2r = redisConnect(REDIS_IP, REDIS_PORT);  // Redis su localhost
    if (c2r == nullptr || c2r->err) {
        std::cerr << "Errore nella connessione a Redis" << std::endl;
        return false;
    }




    // recupera dal DB
    Con2DB db(HOSTNAME, DB_PORT, USERNAME_CUST, PASSWORD_CUST, DB_NAME); // Effettua la connessione al database
    try
    {
        // Prima recuperiamo gli ordini NON consegnati
        sprintf(comando, "SELECT ord.id, ord.datarich, ord.stato, ord.pagamento, ord.indirizzo, ord.totale "
        "FROM ordine ord, customers cst WHERE ord.customer = cst.id AND cst.mail = '%s' AND ord.id NOT IN (SELECT id FROM ordineconse)", mail);
        res = db.ExecSQLtuples(comando);
        rows = PQntuples(res);
        if (rows > 0)
        {
            for (int i = 0; i < rows; i++)
            {
                // Recupera gli attributi degli ordini dalla query sopra svolta...
                int ID = atoi(PQgetvalue(res, i, PQfnumber(res, "id")));
                const char* stato = PQgetvalue(res, i, PQfnumber(res, "stato"));
                unsigned char* data = (unsigned char*) PQgetvalue(res, i, PQfnumber(res, "datarich"));
                time_t time = static_cast<time_t>(std::stoll(reinterpret_cast<char*>(data))); // Converte il timestamp in time_t
                std::string tempo = std::to_string(time); // Converte il tempo in una stringa
                double totale = atof(PQgetvalue(res, i, PQfnumber(res, "totale")));
                const char* pagamento = PQgetvalue(res, i, PQfnumber(res, "pagamento"));
                int indirizzo = atoi(PQgetvalue(res, i, PQfnumber(res, "indirizzo")));

                // Memorizza il prodotto in Redis come hash
                redisCommand(c2r, "HMSET ordine:%d id %d stato %s data %s consegna 0 totale %f pagamento %s indirizzo %d", 
                            ID, ID, stato, tempo.c_str(), totale, pagamento, indirizzo);

                // Aggiungi l'ID del prodotto alla lista associata all'email
                redisCommand(c2r, "RPUSH ordini:%s %d", mail, ID);
            }

        }
        PQclear(res);
        //... poi quelli consegnati
        sprintf(comando, "SELECT ord.id, ord.datarich, ord.stato, ord.pagamento, ord.indirizzo, ord.totale, cons.datacons"
        " FROM ordine ord, customers cst, ordineconse cons WHERE ord.customer = cst.id AND cst.mail = '%s' AND ord.id = cons.id", mail);
        res = db.ExecSQLtuples(comando);
        rows = PQntuples(res);
        if (rows > 0)
        {
            for (int i = 0; i < rows; i++)
            {
                // Recupera gli attributi degli dalla query sopra svolta...
                int ID = atoi(PQgetvalue(res, i, PQfnumber(res, "id")));
                const char* stato = PQgetvalue(res, i, PQfnumber(res, "stato"));
                unsigned char* data = (unsigned char*) PQgetvalue(res, i, PQfnumber(res, "datarich"));
                time_t time = static_cast<time_t>(std::stoll(reinterpret_cast<char*>(data))); // Converte il timestamp in time_t
                std::string tempo = std::to_string(time); // Converte il tempo in una stringa
                unsigned char* data2 = (unsigned char*) PQgetvalue(res, i, PQfnumber(res, "datacons"));
                time_t time2 = static_cast<time_t>(std::stoll(reinterpret_cast<char*>(data2))); // Converte il timestamp in time_t
                std::string tempo2 = std::to_string(time2); // Converte il tempo in una stringa
                double totale = atof(PQgetvalue(res, i, PQfnumber(res, "totale")));
                const char* pagamento = PQgetvalue(res, i, PQfnumber(res, "pagamento"));
                int indirizzo = atoi(PQgetvalue(res, i, PQfnumber(res, "indirizzo")));

                // Memorizza il prodotto in Redis come hash
                redisCommand(c2r, "HMSET ordine:%d id %d stato %s data %s consegna %d totale %f pagamento %s indirizzo %d", 
                            ID, ID, stato, tempo.c_str(), tempo2.c_str(), totale, pagamento);

                // Aggiungi l'ID del prodotto alla lista associata all'email
                redisCommand(c2r, "RPUSH ordini:%s %d", mail, ID);
            }

        }
        PQclear(res);
    }
    catch (...) 
    {
        return false;
    }
    
    redisFree(c2r);
    return true;
}
