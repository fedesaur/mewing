#include "ricercaOrdini.h"


std::tuple<int, Ordine*, Indirizzo*> ricercaOrdini()
{
    int rows;
    char comando[1000];
    std::tuple<int, Ordine*, Indirizzo*> risultato;
    PGresult *res;
	Con2DB db(HOSTNAME, DB_PORT, USERNAME_TRAS, PASSWORD_TRAS, DB_NAME); // Effettua la connessione al database
    try
    {
        //Recupera tutti gli ordini disponibili (non ancora presi in carico da altri trasportatori)
        sprintf(comando, "SELECT ord.id, cst.mail, ord.datarich, ord.stato, ord.pagamento, ind.via, ind.civico, ind.cap, ind.citta, ind.stato AS statoIND, ord.totale "
        "FROM indirizzo ind, customers cst, ordine ord "
        "WHERE ind.id = ord.indirizzo AND ord.customer = cst.id AND ord.id NOT IN (SELECT ordine FROM transord) "
        "AND ord.stato = 'pendente' ORDER BY ord.datarich");

        res = db.ExecSQLtuples(comando);
        rows = PQntuples(res);
        std::cout << rows << std::endl;
        if (rows > 0)
        {
            Ordine* ORDINI = new Ordine[rows];
            Indirizzo* INDIRIZZI = new Indirizzo[rows];
            for (int i = 0; i < rows; i++)
            {
                // Recupera gli attributi dei prodotti dalla query sopra svolta...
                int ID = atoi(PQgetvalue(res, i, PQfnumber(res, "id")));
                const char* mail = PQgetvalue(res, i, PQfnumber(res, "mail"));
                unsigned char* data = (unsigned char*) PQgetvalue(res, i, PQfnumber(res, "datarich"));
                time_t time = static_cast<time_t>(std::stoll(reinterpret_cast<char*>(data))); // Converte il timestamp in time_t
                double totale = atof(PQgetvalue(res, i, PQfnumber(res, "totale")));
                const char* pagamento = PQgetvalue(res, i, PQfnumber(res, "pagamento"));

                const char* via = PQgetvalue(res, i, PQfnumber(res, "via"));
                int civico = atoi(PQgetvalue(res, i, PQfnumber(res, "civico")));
                const char* CAP = PQgetvalue(res, i, PQfnumber(res, "cap"));
                const char* city = PQgetvalue(res, i, PQfnumber(res, "citta"));
                const char* stato = PQgetvalue(res, i, PQfnumber(res, "statoIND"));
            // Assegna gli attributi all'i-esimo Prodotto in prodottiDisponibili
                ORDINI[i].ID = ID;
                ORDINI[i].MailCustomer = mail;
                ORDINI[i].DataRichiesta = time;
                ORDINI[i].Pagamento = pagamento;
                ORDINI[i].Totale = totale;

                INDIRIZZI[i].via = via;
                INDIRIZZI[i].civico = civico;
                INDIRIZZI[i].CAP = CAP;
                INDIRIZZI[i].citta = city;
                INDIRIZZI[i].stato = stato;

            }
           std::get<0>(risultato)=rows;
           std::get<1>(risultato)=ORDINI;
           std::get<2>(risultato)=INDIRIZZI;
        }
        else
        {
            std::tuple<int, Ordine*, Indirizzo*> risultato(0, nullptr, nullptr);
        }
    }
    catch(...)
    {
        std::tuple<int, Ordine*, Indirizzo*> risultato(-1, nullptr, nullptr);
    }
    return risultato;
}


       
            
