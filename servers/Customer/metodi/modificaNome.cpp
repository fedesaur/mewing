#include "modificaNome.h"

bool modificaNomeHttp(const crow::request& req, crow::response& res) {
    int USER_ID;
    PGresult *resDb;
    redisContext *c2r;
    redisReply *reply;
    char comando[1000];
    std::string nome, cognome;
    
    c2r = redisConnect(REDIS_IP, REDIS_PORT);
    Con2DB db(HOSTNAME, DB_PORT, USERNAME_CUST, PASSWORD_CUST, DB_NAME);

    reply = RedisCommand(c2r, "XREVRANGE %s + - COUNT 1", READ_STREAM);
    if (reply == nullptr || reply->type != REDIS_REPLY_ARRAY || reply->elements == 0) {
        res.code = 500;
        res.body = "Errore nel comando Redis o stream vuoto";
        return false;
    }
    std::string id = reply->element[0]->element[1]->element[1]->str;
    USER_ID = std::stoi(id);

    sprintf(comando, "SELECT nome, cognome FROM customers WHERE id = %d", USER_ID);
    resDb = db.ExecSQLtuples(comando);
    if (PQresultStatus(resDb) != PGRES_TUPLES_OK) {
        res.code = 500;
        res.body = "Errore nel recupero dei dati dal database";
        return false;
    }

    nome = PQgetvalue(resDb, 0, PQfnumber(resDb, "nome"));
    cognome = PQgetvalue(resDb, 0, PQfnumber(resDb, "cognome"));

    // Supponiamo che il corpo della richiesta contenga l'azione e i nuovi dati
    std::string action = req.url_params.get("action");
    std::string newValue = req.body;

    if (action == "cambia_nome") {
        if (newValue.length() > 20 || newValue.length() == 0) {
            res.code = 400;
            res.body = "Il nome deve avere massimo 20 caratteri e minimo 1";
            return false;
        }
        sprintf(comando, "UPDATE customers SET nome = '%s' WHERE id = %d", newValue.c_str(), USER_ID);
    } else if (action == "cambia_cognome") {
        if (newValue.length() > 20 || newValue.length() == 0) {
            res.code = 400;
            res.body = "Il cognome deve avere massimo 20 caratteri e minimo 1";
            return false;
        }
        sprintf(comando, "UPDATE customers SET cognome = '%s' WHERE id = %d", newValue.c_str(), USER_ID);
    } else {
        res.code = 400;
        res.body = "Azione non valida";
        return false;
    }

    resDb = db.ExecSQLcmd(comando);
    if (PQresultStatus(resDb) != PGRES_COMMAND_OK) {
        res.code = 500;
        res.body = "Errore nell'aggiornamento del database";
        return false;
    }

    res.code = 200;
    res.body = "Operazione completata con successo";
    PQclear(resDb);
    return true;
}
