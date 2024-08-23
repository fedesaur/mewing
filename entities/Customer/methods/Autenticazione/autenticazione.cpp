#include "autenticazione.h"

bool autentica(int IDConnessione)
{
	redisContext *c2r; // c2r contiene le info sul contesto
	redisReply *reply; // reply contiene le risposte da Redis
	c2r = redisConnect(REDIS_IP, REDIS_PORT); // Effettua la connessione a Redis


	std::cout << "Eccoti l'ID: " << IDConnessione << std::endl;
    reply = RedisCommand(c2r, "XREVRANGE %s + - COUNT 1", CUSTOMER_STREAM);
    if (reply == nullptr || reply->type != REDIS_REPLY_ARRAY || reply->elements == 0)
	{
       std::cerr << "Errore nel comando Redis o stream vuoto" << std::endl;
       return false;
    }

    redisReply* stream = reply -> element[0];
    redisReply* entryFields = stream -> element[1];
    size_t index = IDConnessione*2;
    std::string fieldName = entryFields->element[index]->str; // Chiave
   	std::string received_email = entryFields->element[index+ 1]->str; // Valore

	if (received_email.empty()) std::cerr << "Errore: non è stata trovata nessuna email con la chiave specificata." << std::endl;
	else std::cout << "Email letta dallo stream: " << received_email << std::endl;

    freeReplyObject(reply);
    return true;
}
