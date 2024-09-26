#include "recuperaCarrello.h"

#include "recuperaCarrello.h"

bool recuperaCarrello(const char* mail)
{
    char comando[1000];
    int rows;
    int RIGHE;
    redisReply* productReply = nullptr;
    redisReply* reply = nullptr;
    redisContext *c2r = nullptr;
    PGresult *res = nullptr;

    // Connessione a Redis
    c2r = redisConnect(REDIS_IP, REDIS_PORT);  // Redis su localhost
    if (c2r == nullptr || c2r->err) {
        std::cerr << "Errore nella connessione a Redis: " << (c2r ? c2r->errstr : "null") << std::endl;
        if (c2r) redisFree(c2r);  // Chiudi connessione Redis in caso di errore
        return false;
    }

    // Recupera la lista di ID prodotti da Redis per l'email
    bool onRedis = true; // Controlla che i dati siano nello stream Redis
    reply = (redisReply*)redisCommand(c2r, "LRANGE carrello:%s 0 -1", mail);
    if (reply == nullptr) {
        std::cerr << "Errore nel recupero della lista da Redis" << std::endl;
        redisFree(c2r);
        return false;
    }

    if (reply->type == REDIS_REPLY_ARRAY && reply->elements > 0) // Recupera i prodotti da Redis
    {
        RIGHE = reply->elements;

        for (int i = 0; i < RIGHE; i++) 
        {
            std::string productID = reply->element[i]->str;

            // Recupera il prodotto come hash da Redis
            productReply = (redisReply*)redisCommand(c2r, "HGETALL prodottoCarr:%s", productID.c_str());
    
            // Verifica il risultato del recupero...
            if (productReply == nullptr || productReply->type != REDIS_REPLY_ARRAY || productReply->elements != 12) 
            {
                onRedis = false;
                if (productReply) freeReplyObject(productReply);
                break;
            }
            freeReplyObject(productReply);
        } 

        if (onRedis) // Se ci sono, routes li recupererà poi...
        {
            freeReplyObject(reply);
            redisFree(c2r);
            return true;
        }
    }

    freeReplyObject(reply); // Libera la risposta Redis

    //...altrimenti li recupera dal DB e li immette nello stream
    Con2DB db(HOSTNAME, DB_PORT, USERNAME_CUST, PASSWORD_CUST, DB_NAME); // Effettua la connessione al database
    try
    {
        sprintf(comando, "SELECT ind.id, ind.via, ind.civico, ind.cap, ind.citta, ind.stato "
        "FROM indirizzo ind, custadd cst, customers cus WHERE cst.customer = cus.id AND cus.mail = '%s' AND cst.addr = ind.id", mail);
        res = db.ExecSQLtuples(comando);
        rows = PQntuples(res);
        if (rows > 0)
        {
            for (int i = 0; i < rows; i++)
            {
                // Recupera gli attributi degli dalla query sopra svolta...
                int ID = atoi(PQgetvalue(res, i, PQfnumber(res, "id")));
                const char* descrizione = PQgetvalue(res, i, PQfnumber(res, "descrizione"));
                double prezzo = atof(PQgetvalue(res, i, PQfnumber(res, "prezzo")));
                const char* nome = PQgetvalue(res, i, PQfnumber(res, "nome"));
                const char* fornitore = PQgetvalue(res, i, PQfnumber(res, "nomeF"));
                int quantita = atoi(PQgetvalue(res, i, PQfnumber(res, "quantita")));

                // Memorizza il prodotto in Redis come hash
                redisCommand(c2r, "HMSET prodottoCarr:%d id %d descrizione %s prezzo %f nome %s fornitore %s quantità %d", 
                            ID, ID, descrizione, prezzo, nome, fornitore, quantita);

                // Aggiungi l'ID del prodotto alla lista associata all'email
                redisCommand(c2r, "RPUSH carrello:%s %d", mail, ID);
            }
        }
        PQclear(res);
        redisFree(c2r);  // Chiudi connessione Redis
        return true;
    }
    catch (...) 
    {
        if (res) PQclear(res);
        redisFree(c2r);  // Chiudi connessione Redis
        std::cerr << "Errore durante il recupero dal DB o durante la memorizzazione in Redis" << std::endl;
        return false;
    }
}
