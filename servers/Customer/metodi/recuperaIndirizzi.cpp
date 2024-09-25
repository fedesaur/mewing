#include "recuperaIndirizzi.h"
std::pair<int, Indirizzo*> recuperaIndirizzi(const char* mail)
{
    std::pair<int, Indirizzo*> risultato;
    char comando[1000];
    int rows;
    int RIGHE;
    Indirizzo* INDIRIZZI;
    redisReply* addressReply;
    redisReply* reply;
    redisContext *c2r;
    PGresult *res;

    // Connessione a Redis
    c2r = redisConnect(REDIS_IP, REDIS_PORT);  // Redis su localhost
    if (c2r == nullptr || c2r->err) {
        std::cerr << "Errore nella connessione a Redis" << std::endl;
        risultato.first = -1;
        risultato.second = nullptr;
        return risultato;
    }

    // Recupera la lista di ID prodotti da Redis per l'email
    reply = RedisCommand(c2r, "LRANGE indirizzi:%s 0 -1", mail);
    if (reply->type == REDIS_REPLY_ARRAY && reply->elements > 0) // Recupera gli indirizzi da Redis
    {
        RIGHE = reply->elements;
        INDIRIZZI = new Indirizzo[RIGHE];  // Array dinamico di prodotti
        
        for (int i = 0; i < RIGHE; i++) 
        {
            std::string indirizzoID = reply->element[i]->str;

            // Debug: Stampa l'ID del prodotto che stai per recuperare
            std::cout << "Recuperando indirizzo con ID: " << indirizzoID << std::endl;
            INDIRIZZI[i].ID = std::stoi(indirizzoID);

            // Recupera il prodotto come hash da Redis
            addressReply = RedisCommand(c2r, "HGETALL indirizzo:%s", indirizzoID.c_str());
    
            // Verifica il risultato del recupero...
            if ( addressReply->type == REDIS_REPLY_ARRAY && addressReply->elements == 10) // 5 dati Richiesti: Via, Civico, CAP, Città, Stato
            { //... e asssocia i valori dell'indirizzo recuperato dallo stream Redis ad un oggetto Indirizzo 
        
                INDIRIZZI[i].via = (addressReply->element[1]->str);
                INDIRIZZI[i].civico = std::atoi(addressReply->element[3]->str);
                INDIRIZZI[i].CAP = (addressReply->element[5]->str);
                INDIRIZZI[i].citta = (addressReply->element[7]->str);
                INDIRIZZI[i].stato = (addressReply->element[9]->str);
            }
            freeReplyObject(addressReply);
        }
        risultato.first = RIGHE;
        risultato.second = INDIRIZZI;
        freeReplyObject(reply);
        redisFree(c2r);
        return risultato;
    }

    // Se gli indirizzi non sono trovati su Redis, recupera dal DB
    Con2DB db(HOSTNAME, DB_PORT, USERNAME_CUST, PASSWORD_CUST, DB_NAME); // Effettua la connessione al database
    try
    {
        sprintf(comando, "SELECT ind.id, ind.via, ind.civico, ind.cap, ind.citta, ind.stato "
        "FROM indirizzo ind, custadd cst, customers cus WHERE cst.customer = cus.id AND cus.mail = '%s' AND cst.addr = ind.id", mail);
        res = db.ExecSQLtuples(comando);
        rows = PQntuples(res);
        if (rows > 0)
        {
            INDIRIZZI = new Indirizzo[rows];

            for (int i = 0; i < rows; i++)
            {
                // Recupera gli attributi degli dalla query sopra svolta...
                int ID = atoi(PQgetvalue(res, i, PQfnumber(res, "id")));
                const char* via = PQgetvalue(res, i, PQfnumber(res, "via"));
                int civico = atoi(PQgetvalue(res, i, PQfnumber(res, "civico")));
                const char* CAP = PQgetvalue(res, i, PQfnumber(res, "cap"));
                const char* citta = PQgetvalue(res, i, PQfnumber(res, "citta"));
                const char* stato = PQgetvalue(res, i, PQfnumber(res, "stato"));

                // e li assegna all'i-esimo Indirizzo in indirizzi
                INDIRIZZI[i].ID = ID;
                INDIRIZZI[i].via = via;
                INDIRIZZI[i].civico = civico;
                INDIRIZZI[i].CAP = CAP;
                INDIRIZZI[i].citta = citta;
                INDIRIZZI[i].stato = stato;

                // Memorizza il prodotto in Redis come hash
                redisCommand(c2r, "HMSET indirizzo:%d id %d nome %s via %s civico %d cap %s città %s stato %s", 
                            ID, ID, via, civico, CAP, citta, stato);

                // Aggiungi l'ID del prodotto alla lista associata all'email
                redisCommand(c2r, "RPUSH indirizzi:%s %d", mail, ID);
            }

            risultato.first = rows;
            risultato.second = INDIRIZZI;
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
    
    redisFree(c2r);  // Chiudi connessione Redis
    return risultato;
}
