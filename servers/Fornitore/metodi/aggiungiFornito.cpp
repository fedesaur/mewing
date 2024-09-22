#include "aggiungiFornito.h"

bool aggiungiFornito(const char* email, const char* nomeProdotto, const char* descrizioneProdotto, double prezzoProdotto)
{
    int PRODUCER_ID;
    PGresult *res;
    redisContext *c2r;
	redisReply *reply;
    char comando[1000];
    char userMail[100];

	c2r = redisConnect(REDIS_IP, REDIS_PORT);
	if (c2r == nullptr || c2r->err) {
        std::cerr << "Errore di connessione a Redis: " << (c2r ? c2r->errstr : "Errore sconosciuto") << std::endl;
        return false;
    }
    
	Con2DB db(HOSTNAME, DB_PORT, USERNAMEP, PASSWORDP, DB_NAME);

    ID = atoi(PQgetvalue(res, 0, PQfnumber(res, "id")));
	reply = RedisCommand(c2r, "XADD %s * %s %d", WRITE_STREAM, mail, ID);
    assertReplyType(c2r, reply, REDIS_REPLY_STRING);
    freeReplyObject(reply);

    reply = RedisCommand(c2r, "XREVRANGE %s + - COUNT 1", WRITE_STREAM);
    if (reply == nullptr || reply->type != REDIS_REPLY_ARRAY || reply->elements == 0)
    {
        std::cerr << "Errore nel comando Redis o stream vuoto" << std::endl;
        return false;
    }
    ReadStreamNumMsgID(reply, 0, 0, userMail);
    if (userMail != email) return false; // Se l'email a cui è associato l'ID non corrisponde, impedisce l'operazione

    std::string id = reply->element[0]->element[1]->element[1]->str; 
    PRODUCER_ID = stoi(id);

	sprintf(comando, "INSERT INTO prodotto(descrizione, prezzo, nome, fornitore) VALUES('%s', %f, '%s', %d)",
    descrizioneProdotto, prezzoProdotto, nomeProdotto, PRODUCER_ID);
    try
    {
        res = db.ExecSQLcmd(comando);
        PQclear(res);
        return true;
    }
    catch(...)
    {
        PQclear(res);
        return false;
    }
}
