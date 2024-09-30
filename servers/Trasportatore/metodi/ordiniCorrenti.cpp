#include "ordiniCorrenti.h"

bool ordiniCorrenti(const char* piva, int trasporterID)  
{
    int rows;
    char comando[1000];
    PGresult *res;
    redisContext *c2r; // c2r contiene le info sul contesto
	redisReply *reply; // reply contiene le risposte da Redis

	c2r = redisConnect(REDIS_IP, REDIS_PORT); // Effettua la connessione a Redis
	Con2DB db(HOSTNAME, DB_PORT, USERNAME_TRAS, PASSWORD_TRAS, DB_NAME); // Effettua la connessione al database

    reply = RedisCommand(c2r, "DEL correnti:%s", piva);
    assertReply(c2r, reply);
    freeReplyObject(reply);

    // Seleziona tutti i corrieri e gli ordini da loro presi in carico
    sprintf(comando, "SELECT ord.id, cst.mail, ord.datarich, ord.stato, ord.pagamento, ord.totale, cor.id AS CorID, cor.nome, cor.cognome "
    "FROM customers cst, ordine ord, consegna cons, corriere cor WHERE ord.id = cons.ordine "
    "AND cor.id = cons.corriere AND ord.customer = cst.id AND cor.azienda = %d AND ord.id NOT IN (SELECT id FROM ordineconse) "
    "ORDER BY cor.id", trasporterID);
    try
    {
        //Recupera tutti gli ordini presi in carico dal trasportatore e non ancora consegnati
        res = db.ExecSQLtuples(comando);
        rows = PQntuples(res);
        if (rows > 0)
        {
            for (int i = 0; i < rows; i++)
            {
                // Recupera gli attributi degli ordini...
                int ID = atoi(PQgetvalue(res, i, PQfnumber(res, "id")));
                const char* mail = PQgetvalue(res, i, PQfnumber(res, "mail"));
                unsigned char* data = (unsigned char*) PQgetvalue(res, i, PQfnumber(res, "datarich"));
                time_t time = static_cast<time_t>(std::stoll(reinterpret_cast<char*>(data))); // Converte il timestamp in time_t
                std::string tempo = std::to_string(time); // Converte il tempo in una stringa
                const char* statoOrd = PQgetvalue(res, i, PQfnumber(res, "stato"));
                const char* paga = PQgetvalue(res, i, PQfnumber(res, "pagamento"));
                double totale = atof(PQgetvalue(res, i, PQfnumber(res, "totale")));

                //...e dei corrieri a loro assegnati (ci saranno duplicati)
                int IDCor = atoi(PQgetvalue(res, i, PQfnumber(res, "CorID")));
                const char* nome = PQgetvalue(res, i, PQfnumber(res, "nome"));
                const char* cognome = PQgetvalue(res, i, PQfnumber(res, "cognome"));

                //...e li condivide sullo stream Redis
                redisCommand(c2r, "HMSET ordine:%d id %d mail %s data %s totale %f pagamento %s stato %s IDCor %d nomeCor %s cognomeCor %s", 
                            ID, ID, mail, tempo.c_str(), totale, paga, statoOrd, IDCor, nome, cognome);
                
                // Aggiungi l'ID del prodotto alla lista associata all'email
                redisCommand(c2r, "RPUSH correnti:%s %d", piva, ID);
            }
        }
        PQclear(res);
        redisFree(c2r);
        return true;
    }
    catch(...)
    {
        PQclear(res);
        redisFree(c2r);
        return false;
    }
}

       
            
