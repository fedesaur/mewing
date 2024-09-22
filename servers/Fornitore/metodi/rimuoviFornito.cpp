#include "rimuoviFornito.h"

bool rimuoviFornito(const char* email, int productID)
{
    int PRODUCER_ID;
    char comando[1000];
    const char* userMail;
    Prodotto* FORNITI;
    PGresult *res;
    redisContext *c2r;
    redisReply *reply;

    Con2DB db(HOSTNAME, DB_PORT, USERNAMEP, PASSWORDP, DB_NAME);
	c2r = redisConnect(REDIS_IP, REDIS_PORT);
	if (c2r == nullptr || c2r->err) {
        std::cerr << "Errore di connessione a Redis: " << (c2r ? c2r->errstr : "Errore sconosciuto") << std::endl;
        return false;
    }
    
    reply = RedisCommand(c2r, "XREVRANGE %s + - COUNT 1", WRITE_STREAM);
    if (reply == nullptr || reply->type != REDIS_REPLY_ARRAY || reply->elements == 0)
    {
        std::cerr << "Errore nel comando Redis o stream vuoto" << std::endl;
        return false;
    }

    // Recupera l'email dell'utente dallo stream Redis
    std::string mail = reply->element[0]->element[1]->element[0]->str;
    userMail = mail.c_str();
    if (strcmp(userMail,email) != 0) return false; // Se l'email a cui è associato l'ID non corrisponde, impedisce l'operazione
    std::string id = reply->element[0]->element[1]->element[1]->str; 
    PRODUCER_ID = stoi(id);

    try
    {
        sprintf(comando, "DELETE FROM prodotto WHERE id = %d AND fornitore = %d", productID, PRODUCER_ID);
        res = db.ExecSQLcmd(comando);
        PQclear(res);
        return true;
    }
    catch(...)
    {
        PQclear(res);
        // Se ci sono errori nella query, vengono catturati da catch
        return false;
    }
    return true;
}
