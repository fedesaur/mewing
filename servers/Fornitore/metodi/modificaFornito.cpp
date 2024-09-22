#include "modificaFornito.h"

bool modificaFornito(const char* email, const char* nomeProdotto, const char* descrizioneProdotto, double prezzoProdotto, int productID)
{
    int PRODUCER_ID;
    const char* userMail;
    char comando[1000];
    redisContext *c2r; // c2r contiene le info sul contesto
	redisReply *reply; // reply contiene le risposte da Redis
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
        sprintf(comando, "UPDATE prodotto SET descrizione = '%s' AND prezzo = %f AND nome = '%s' WHERE fornitore = %d AND id = %d",
        descrizioneProdotto, prezzoProdotto, nomeProdotto, PRODUCER_ID, productID);
        res = db.ExecSQLcmd(comando);
        PQclear(res);
        return true;
    }
    catch(...)
    {
        return false;
    }
}