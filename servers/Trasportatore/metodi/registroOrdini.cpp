#include "registroOrdini.h"

std::pair<int, Ordine*> registroOrdini(int clientSocket)
{
    int COURIER_ID;
    int rows;
    char comando[1000];
    PGresult *res;
    redisContext *c2r; // c2r contiene le info sul contesto
	redisReply *reply; // reply contiene le risposte da Redis

	c2r = redisConnect(REDIS_IP, REDIS_PORT); // Effettua la connessione a Redis
	Con2DB db(HOSTNAME, DB_PORT, USERNAME, PASSWORD, DB_NAME); // Effettua la connessione al database

    reply = RedisCommand(c2r, "XREVRANGE %s + - COUNT 1", READ_STREAM);
    if (reply == nullptr || reply->type != REDIS_REPLY_ARRAY || reply->elements == 0)
    {
        std::cerr << "Errore nel comando Redis o stream vuoto" << std::endl;
    }

    std::string id = reply->element[0]->element[1]->element[1]->str; 
    COURIER_ID = atoi(id.c_str()); // ID Corriere
    sprintf(comando, "SELECT * FROM ordine");
    try
    {
        //Recupera tutti gli ordini disponibili (non ancora cons)
        res = db.ExecSQLtuples(comando);
        rows = PQntuples(res);
        if (rows > 0)
        {
            Ordine* ordiniDisponibili = new Ordine[rows];
            for (int i = 0; i < rows; i++)
            {
                // Recupera gli attributi dei prodotti dalla query sopra svolta...
                int ID = atoi(PQgetvalue(res, i, PQfnumber(res, "id")));
                const char* nome = PQgetvalue(res, i, PQfnumber(res, "nome"));
                unsigned char* data = (unsigned char*) PQgetvalue(res, i, PQfnumber(res, "datarich"));
                time_t time = _atoi64((char*)data); // Converte il timestamp in time_t
                double prezzo = atof(PQgetvalue(res, i, PQfnumber(res, "prezzo")));
                const char* nome = 
                const char* fornitore = PQgetvalue(res, i, PQfnumber(res, "nomeF"));

            // Assegna gli attributi all'i-esimo Prodotto in prodottiDisponibili
                prodottiDisponibili[i].ID = ID;
                prodottiDisponibili[i].descrizione = descrizione;
                prodottiDisponibili[i].prezzo = prezzo;
                prodottiDisponibili[i].nome = nome;
                prodottiDisponibili[i].fornitore = fornitore;
            }
        }
        else
        {
            std::string vuoto = "Non ci sono ordini disponibili!\n";
            send(clientSocket, vuoto.c_str(), vuoto.length(), 0);
            return true;
        }
    }
    catch(...)
    {
        std::string errore = "C'è stato un problema con il database\n";
        send(clientSocket, errore.c_str(), errore.length(), 0);
        return false;
    }