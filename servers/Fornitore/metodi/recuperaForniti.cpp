std::pair<int, Prodotto*> recuperaForniti(const char* mail)
{
    std::pair<int, Prodotto*> risultato;
    int rows;
    PGresult *res;

    // Connessione a Redis
    redisContext *redis = redisConnect("127.0.0.1", 6379);  // Redis su localhost
    if (redis == nullptr || redis->err) {
        std::cerr << "Errore nella connessione a Redis" << std::endl;
        risultato.first = -1;
        risultato.second = nullptr;
        return risultato;
    }

    // Recupera la lista di ID prodotti da Redis per l'email
    redisReply* reply = (redisReply*)redisCommand(redis, "LRANGE prodotti:%s 0 -1", mail);
    if (reply->type == REDIS_REPLY_ARRAY && reply->elements > 0) {
        // Recupera i prodotti da Redis
        rows = reply->elements;
        Prodotto* forniti = new Prodotto[rows];

        for (int i = 0; i < rows; i++) {
            std::string prodottoID = reply->element[i]->str;

            // Recupera il prodotto come hash
            redisReply* prodottoReply = (redisReply*)redisCommand(redis, "HGETALL prodotto:%s", prodottoID.c_str());
            if (prodottoReply->type == REDIS_REPLY_ARRAY && prodottoReply->elements == 8) {
                // Associa i valori del prodotto a un oggetto Prodotto
                forniti[i].ID = std::stoi(prodottoReply->element[1]->str);
                forniti[i].nome = strdup(prodottoReply->element[3]->str);  // Copia sicura della stringa
                forniti[i].descrizione = strdup(prodottoReply->element[5]->str);
                forniti[i].prezzo = std::stod(prodottoReply->element[7]->str);
            }
            freeReplyObject(prodottoReply);
        }

        risultato.first = rows;
        risultato.second = forniti;
        freeReplyObject(reply);
        redisFree(redis);
        return risultato;
    }

    // Se i prodotti non sono trovati su Redis, recupera dal DB
    Con2DB db(HOSTNAME, DB_PORT, USERNAMEP, PASSWORDP, DB_NAME);  // Connessione al DB
    try
    {
        char comando[1000];
        sprintf(comando, "SELECT pr.id, pr.descrizione, pr.prezzo, pr.nome FROM prodotto pr, fornitore fr WHERE pr.fornitore = fr.id AND fr.mail = '%s' ", mail);
        res = db.ExecSQLtuples(comando);
        rows = PQntuples(res);
        if (rows > 0)
        {
            Prodotto* forniti = new Prodotto[rows];

            for (int i = 0; i < rows; i++)
            {
                int ID = atoi(PQgetvalue(res, i, PQfnumber(res, "id")));
                const char* descrizione = PQgetvalue(res, i, PQfnumber(res, "descrizione"));
                double prezzo = atof(PQgetvalue(res, i, PQfnumber(res, "prezzo")));
                const char* nome = PQgetvalue(res, i, PQfnumber(res, "nome"));

                // Assegna gli attributi all'i-esimo Prodotto in forniti
                forniti[i].ID = ID;
                forniti[i].descrizione = strdup(descrizione);  // Copia sicura della stringa
                forniti[i].prezzo = prezzo;
                forniti[i].nome = strdup(nome);

                // Memorizza il prodotto in Redis come hash
                redisCommand(redis, "HMSET prodotto:%d id %d nome %s descrizione %s prezzo %f", 
                            ID, ID, nome, descrizione, prezzo);

                // Aggiungi l'ID del prodotto alla lista associata all'email
                redisCommand(redis, "RPUSH prodotti:%s %d", mail, ID);
            }

            risultato.first = rows;
            risultato.second = forniti;
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
    
    redisFree(redis);  // Chiudi connessione Redis
    return risultato;
}
