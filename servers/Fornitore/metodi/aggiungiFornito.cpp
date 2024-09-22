#include "aggiungiFornito.h"

bool aggiungiFornito(const char* email, const char* nomeProdotto, const char* descrizioneProdotto, double prezzoProdotto)
{
    int PRODUCER_ID;
    PGresult *res;
    redisContext *c2r;
	redisReply *reply;
    char comando[1000];
    const char* userMail;

	c2r = redisConnect(REDIS_IP, REDIS_PORT);
	if (c2r == nullptr || c2r->err) {
        std::cerr << "Errore di connessione a Redis: " << (c2r ? c2r->errstr : "Errore sconosciuto") << std::endl;
        return false;
    }
    
	Con2DB db(HOSTNAME, DB_PORT, USERNAMEP, PASSWORDP, DB_NAME);

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
        sprintf(comando, "INSERT INTO prodotto(descrizione, prezzo, nome, fornitore) VALUES('%s', %f, '%s', %d)",
        descrizioneProdotto, prezzoProdotto, nomeProdotto, PRODUCER_ID);
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
