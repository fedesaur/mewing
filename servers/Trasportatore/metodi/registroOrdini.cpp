#include "registroOrdini.h"

bool registroOrdini(const char* piva, int trasporterID)  
{
    char comando[1000];
    int RIGHE_CORRENTI;
    int RIGHE_CONSEGNATI;
    PGresult *res;
    redisContext *c2r; // c2r contiene le info sul contesto
	redisReply *reply; // reply contiene le risposte da Redis

	Con2DB db(HOSTNAME, DB_PORT, USERNAME_TRAS, PASSWORD_TRAS, DB_NAME); // Effettua la connessione al database
    c2r = redisConnect(REDIS_IP, REDIS_PORT);  // Redis su localhost
    if (c2r == nullptr || c2r->err) {
        std::cerr << "Errore nella connessione a Redis" << std::endl;
        return false;
    }

    reply = RedisCommand(c2r, "DEL registroOrdini:%s", piva);
    assertReply(c2r, reply);
    freeReplyObject(reply);

    try
    {
        // Recupera gli ordini in corso e non ancora consegnati
        sprintf(comando, "SELECT ord.id, cst.mail, ord.datarich, ord.stato, ord.pagamento, ord.totale, cor.id AS CorID, cor.nome, cor.cognome "
        "FROM customers cst, ordine ord, consegna cons, corriere cor WHERE ord.id = cons.ordine "
        "AND cor.id = cons.corriere AND ord.customer = cst.id AND cor.azienda = %d AND ord.id NOT IN (SELECT id FROM ordineconse) "
        "ORDER BY ord.datarich", trasporterID);
        res = db.ExecSQLtuples(comando);
        RIGHE_CORRENTI = PQntuples(res);
        if (RIGHE_CORRENTI > 0)
        {
            for (int i = 0; i < RIGHE_CORRENTI; i++)
            {
                // Recupera gli attributi degli ordini e i corrieri della query sopra svolta...
                int ID = atoi(PQgetvalue(res, i, PQfnumber(res, "id")));
                const char* mail = PQgetvalue(res, i, PQfnumber(res, "mail"));
                unsigned char* data = (unsigned char*) PQgetvalue(res, i, PQfnumber(res, "datarich"));
                time_t time = static_cast<time_t>(std::stoll(reinterpret_cast<char*>(data))); // Converte il timestamp in time_t
                std::string tempo = std::to_string(time); // Converte il tempo in una stringa
                const char* statoOrd = PQgetvalue(res, i, PQfnumber(res, "stato"));
                const char* paga = PQgetvalue(res, i, PQfnumber(res, "pagamento"));
                double totale = atof(PQgetvalue(res, i, PQfnumber(res, "totale")));
                int IDCor = atoi(PQgetvalue(res, i, PQfnumber(res, "CorID")));
                const char* nome = PQgetvalue(res, i, PQfnumber(res, "nome"));
                const char* cognome = PQgetvalue(res, i, PQfnumber(res, "cognome"));

                                // Memorizza il prodotto in Redis come hash
                redisCommand(c2r, "HMSET ordineRegistrato:%d id %d mail %s stato %s richiesto %s consegnato 0 totale %f pagamento %s IDCor %d nome %s cognome %s", 
                            ID, ID, mail, statoOrd, tempo.c_str(), totale, paga, IDCor, nome, cognome);

                // Aggiungi l'ID del prodotto alla lista associata all'email
                redisCommand(c2r, "RPUSH registroOrdini:%s %d", piva, ID);
            }
        }
        PQclear(res);
        // Recupera gli ordini consegnati
        sprintf(comando, "SELECT ord.id, cst.mail, ord.datarich, orc.datacons, ord.stato, ord.pagamento, ord.totale, cor.id AS CorID, cor.nome, cor.cognome "
        "FROM customers cst, ordine ord, consegna cons, corriere cor, ordineconse orc WHERE ord.id = cons.ordine "
        "AND cor.id = cons.corriere AND ord.customer = cst.id AND cor.azienda = %d AND ord.id = orc.id "
        "ORDER BY orc.datacons", trasporterID);
        res = db.ExecSQLtuples(comando);
        RIGHE_CONSEGNATI = PQntuples(res);

        for (int i = 0; i < RIGHE_CONSEGNATI; i++) // Dopodichè copia gli ordini consegnati al suo interno
        {
            // Recupera gli attributi degli ordini e i corrieri della query sopra svolta...
            int ID = atoi(PQgetvalue(res, i, PQfnumber(res, "id")));
            const char* mail = PQgetvalue(res, i, PQfnumber(res, "mail"));
            unsigned char* data1 = (unsigned char*) PQgetvalue(res, i, PQfnumber(res, "datarich"));
            time_t time = static_cast<time_t>(std::stoll(reinterpret_cast<char*>(data1))); // Converte il timestamp in time_t
            std::string tempo = std::to_string(time); // Converte il tempo in una stringa
            unsigned char* data2 = (unsigned char*) PQgetvalue(res, i, PQfnumber(res, "datarich"));
            time_t timeCons = static_cast<time_t>(std::stoll(reinterpret_cast<char*>(data2))); // Converte il timestamp in time_t
            std::string tempo2 = std::to_string(timeCons); // Converte il tempo in una stringa
            const char* statoOrd = PQgetvalue(res, i, PQfnumber(res, "stato"));
            const char* paga = PQgetvalue(res, i, PQfnumber(res, "pagamento"));
            double totale = atof(PQgetvalue(res, i, PQfnumber(res, "totale")));
            int IDCor = atoi(PQgetvalue(res, i, PQfnumber(res, "CorID")));
            const char* nome = PQgetvalue(res, i, PQfnumber(res, "nome"));
            const char* cognome = PQgetvalue(res, i, PQfnumber(res, "cognome"));

            redisCommand(c2r, "HMSET ordineRegistrato:%d id %d mail %s stato %s richiesto %s consegnato %s totale %f pagamento %s IDCor %d nome %s cognome %s", 
                            ID, ID, mail, statoOrd, tempo.c_str(), tempo2.c_str(), totale, paga, IDCor, nome, cognome);

            // Aggiungi l'ID del prodotto alla lista associata all'email
            redisCommand(c2r, "RPUSH registroOrdini:%s %d", piva, ID);
        }
        PQclear(res);
        redisFree(c2r); //Chiude la connessione con Redis
        return true;
    }
    catch(...)
    {
        PQclear(res);
        redisFree(c2r);
        return false;
    }
}