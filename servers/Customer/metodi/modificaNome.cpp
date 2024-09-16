#include "modificaNome.h"

bool modificaNomeHttp(const Pistache::Http::Request& req, Pistache::Http::ResponseWriter response) {
    int USER_ID;
    PGresult *resDb;
    redisContext *c2r;
    redisReply *reply;
    char comando[1000];
    std::string nome, cognome;
    
    c2r = redisConnect(REDIS_IP, REDIS_PORT);
    if (c2r == nullptr || c2r->err) {
        response.send(Pistache::Http::Code::Internal_Server_Error, "Errore nella connessione a Redis");
        return false;
    }

    Con2DB db(HOSTNAME, DB_PORT, USERNAME_CUST, PASSWORD_CUST, DB_NAME);

    reply = RedisCommand(c2r, "XREVRANGE %s + - COUNT 1", READ_STREAM);
    if (reply == nullptr || reply->type != REDIS_REPLY_ARRAY || reply->elements == 0) {
        response.send(Pistache::Http::Code::Internal_Server_Error, "Errore nel comando Redis o stream vuoto");
        return false;
    }
    std::string id = reply->element[0]->element[1]->element[1]->str;
    USER_ID = std::stoi(id);

    sprintf(comando, "SELECT nome, cognome FROM customers WHERE id = %d", USER_ID);
    resDb = db.ExecSQLtuples(comando);
    if (PQresultStatus(resDb) != PGRES_TUPLES_OK) {
        response.send(Pistache::Http::Code::Internal_Server_Error, "Errore nel recupero dei dati dal database");
        return false;
    }

    nome = PQgetvalue(resDb, 0, PQfnumber(resDb, "nome"));
    cognome = PQgetvalue(resDb, 0, PQfnumber(resDb, "cognome"));

    // Supponiamo che il corpo della richiesta contenga l'azione e i nuovi dati
    auto actionOpt = req.query().get("action");
    std::string action = actionOpt ? actionOpt.value() : "";
    std::string newValue = req.body();

    if (action == "cambia_nome") {
        if (newValue.length() > 20 || newValue.length() == 0) {
            response.send(Pistache::Http::Code::Bad_Request, "Il nome deve avere massimo 20 caratteri e minimo 1");
            return false;
        }
        sprintf(comando, "UPDATE customers SET nome = '%s' WHERE id = %d", newValue.c_str(), USER_ID);
    } else if (action == "cambia_cognome") {
        if (newValue.length() > 20 || newValue.length() == 0) {
            response.send(Pistache::Http::Code::Bad_Request, "Il cognome deve avere massimo 20 caratteri e minimo 1");
            return false;
        }
        sprintf(comando, "UPDATE customers SET cognome = '%s' WHERE id = %d", newValue.c_str(), USER_ID);
    } else {
        response.send(Pistache::Http::Code::Bad_Request, "Azione non valida");
        return false;
    }

    resDb = db.ExecSQLcmd(comando);
    if (PQresultStatus(resDb) != PGRES_COMMAND_OK) {
        response.send(Pistache::Http::Code::Internal_Server_Error, "Errore nell'aggiornamento del database");
        return false;
    }

    response.send(Pistache::Http::Code::Ok, "Operazione completata con successo");
    PQclear(resDb);
    return true;
}
