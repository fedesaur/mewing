#include "dettagliOrdine.h"

bool dettagliOrdine(int orderID)
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
    reply = RedisCommand(c2r, "DEL dettagli:%d", orderID);
    assertReply(c2r, reply);
    freeReplyObject(reply);

    // Recupera dal DB
    Con2DB db(HOSTNAME, DB_PORT, USERNAME_TRAS, PASSWORD_TRAS, DB_NAME); // Effettua la connessione al database
    try
    {
        // Prima recuperiamo gli ordini NON consegnati
        sprintf(comando, "SELECT cst.mail, ord.datarich, ord.stato, ord.pagamento, ind.via, ind.civico, ind.cap, ind.citta, ind.stato AS statoIND, ord.totale "
        "FROM indirizzo ind, customers cst, ordine ord "
        "WHERE ind.id = ord.indirizzo AND ord.customer = cst.id AND ord.id = %d", orderID);
        res = db.ExecSQLtuples(comando);
        if (PQntuples(res) == 0) return false;
        
        // Recupera gli attributi dell'ordine...
        const char* mail = PQgetvalue(res, 0, PQfnumber(res, "mail"));
        unsigned char* data = (unsigned char*) PQgetvalue(res, 0, PQfnumber(res, "datarich"));
        time_t time = static_cast<time_t>(std::stoll(reinterpret_cast<char*>(data))); // Converte il timestamp in time_t
        std::string tempo = std::to_string(time); // Converte il tempo in una stringa
        const char* statoOrd = PQgetvalue(res, 0, PQfnumber(res, "stato"));
        const char* paga = PQgetvalue(res, 0, PQfnumber(res, "pagamento"));
        
        //...e dell'indirizzo di consegna
        const char* via = PQgetvalue(res, 0, PQfnumber(res, "via"));
        int civico = atoi(PQgetvalue(res, 0, PQfnumber(res, "civico")));
        const char* CAP = PQgetvalue(res, 0, PQfnumber(res, "cap"));
        const char* city = PQgetvalue(res, 0, PQfnumber(res, "citta"));
        const char* stato = PQgetvalue(res, 0, PQfnumber(res, "statoIND"));
        double totale = atof(PQgetvalue(res, 0, PQfnumber(res, "totale")));
        
        redisCommand(c2r, "HMSET dettaglio:%d id %d mail %s data %s statoOrd %s totale %f pagamento %s via %s civico %d CAP %s city %s stato %s", 
                            orderID, orderID, mail, tempo.c_str(), statoOrd, totale, paga, via, civico, CAP, city, stato);

        // Aggiungi l'ID del prodotto alla lista associata all'ID del prodotto
        redisCommand(c2r, "RPUSH dettagli:%d %d", orderID, orderID);
        PQclear(res);
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

            redisCommand(c2r, "HMSET dettaglio:%d id %d nome %s descrizione %s fornitore %s prezzo %f quantita %d", IDProd, IDProd, nome, descrizione, fornitore, prezzo, quantita);

            // Aggiungi l'ID del prodotto alla lista associata all'ID dell'ordine
            redisCommand(c2r, "RPUSH dettagli:%d %d", orderID, IDProd);
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