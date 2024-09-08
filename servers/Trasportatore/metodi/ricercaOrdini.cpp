#include "ricercaOrdini.h"

bool ricercaOrdini(int clientSocket)
{
    int COURIER_ID;
    int rows;
    char comando[1000];
    PGresult *res;
    redisContext *c2r; // c2r contiene le info sul contesto
	redisReply *reply; // reply contiene le risposte da Redis

	c2r = redisConnect(REDIS_IP, REDIS_PORT); // Effettua la connessione a Redis
	Con2DB db(HOSTNAME, DB_PORT, USERNAMEC, PASSWORDC, DB_NAME); // Effettua la connessione al database

    reply = RedisCommand(c2r, "XREVRANGE %s + - COUNT 1", READ_STREAM);
    if (reply == nullptr || reply->type != REDIS_REPLY_ARRAY || reply->elements == 0)
    {
        std::cerr << "Errore nel comando Redis o stream vuoto" << std::endl;
    }

    std::string id = reply->element[0]->element[1]->element[1]->str; 
    COURIER_ID = atoi(id.c_str()); // ID Customer
    sprintf(comando, "SELECT ord.id, cst.nome, ord.datarich, ord.stato, ord.pagamento, ord.indirizzo, ord.totale "
    "FROM Ordine ord, Customer cst WHERE cst.id = ord.customer AND ord.id NOT IN (SELECT id FROM ordineconse) AND ord.id NOT IN (SELECT ordine FROM consegna)");
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
                const char* mail = PQgetvalue(res, i, PQfnumber(res, "customer"));
                unsigned char* data = (unsigned char*) PQgetvalue(res, i, PQfnumber(res, "datarich"));
                time_t time = static_cast<time_t>(std::stoll(reinterpret_cast<char*>(data))); // Converte il timestamp in time_t
                double prezzo = atof(PQgetvalue(res, i, PQfnumber(res, "prezzo")));
                const char* pagamento = PQgetvalue(res, i, PQfnumber(res, "pagamento"));;
                int indirizzo = atoi(PQgetvalue(res, i, PQfnumber(res, "indirizzo")));

            // Assegna gli attributi all'i-esimo Prodotto in prodottiDisponibili
                ordiniDisponibili[i].ID = ID;
                ordiniDisponibili[i].MailCustomer = mail;
                ordiniDisponibili[i].DataRichiesta = time;
                ordiniDisponibili[i].Pagamento = pagamento;
                ordiniDisponibili[i].Indirizzo = indirizzo;
                ordiniDisponibili[i].Totale = prezzo;
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
    
    return true;
}


       
            
