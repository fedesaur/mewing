#include "autenticazione.h"

bool autentica(int IDConnessione)
{
	redisContext *c2r; // c2r contiene le info sul contesto
	redisReply *reply; // reply contiene le risposte da Redis
	/*
         Now read the stream for authentication confirmation
         Start reading from the `entryID`
    */
    reply = RedisCommand(c2r, "XREVRANGE %s + - COUNT 1", CUSTOMER_STREAM);
    if (reply == nullptr || reply->type != REDIS_REPLY_ARRAY || reply->elements == 0)
	{
       std::cerr << "Errore nel comando Redis o stream vuoto" << std::endl;
       return false;
    }

    redisReply* stream = reply -> element[0];
    redisReply* entryFields = stream -> element[1];
    std::string fieldName = entryFields->element[IDConnessione*2]->str; // Chiave
   	std::string received_email = entryFields->element[IDConnessione*2 + 1]->str; // Valore

	if (received_email.empty()) std::cerr << "Errore: non è stata trovata nessuna email con la chiave specificata." << std::endl;
	else std::cout << "Email letta dallo stream: " << received_email << std::endl;

    freeReplyObject(reply);
    return true;
}