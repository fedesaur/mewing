#include "gestioneOrdini.h"


void dettagliOrdine(int clientSocket, int ordineID)  
{
    int rows;
    char comando[1000];
    PGresult *res;
	Con2DB db(HOSTNAME, DB_PORT, USERNAME_TRAS, PASSWORD_TRAS, DB_NAME); // Effettua la connessione al database

    try
    {
        //Recupera tutti gli ordini presi in carico dal trasportatore e non ancora consegnati
        sprintf(comando, "SELECT cst.mail, ord.datarich, ord.stato, ord.pagamento, ind.via, ind.civico, ind.cap, ind.città, ind.stato AS statoIND, ord.totale"
        "FROM indirizzo ind, customers cst, ordine ord "
        "WHERE ind.id = ord.indirizzo AND ord.customer = cst.id AND ord.id = %d", ordineID);
        res = db.ExecSQLtuples(comando);
        if (PQntuples(res) == 0) return;
        // Recupera gli attributi dell'ordine...
        const char* mail = PQgetvalue(res, 0, PQfnumber(res, "mail"));
        unsigned char* data = (unsigned char*) PQgetvalue(res, 0, PQfnumber(res, "datarich"));
        time_t time = static_cast<time_t>(std::stoll(reinterpret_cast<char*>(data))); // Converte il timestamp in time_t
        const char* statoOrd = PQgetvalue(res, 0, PQfnumber(res, "stato"));
        const char* paga = PQgetvalue(res, 0, PQfnumber(res, "pagamento"));
        
        //...e dell'indirizzo di consegna
        const char* via = PQgetvalue(res, 0, PQfnumber(res, "via"));
        int civico = atoi(PQgetvalue(res, 0, PQfnumber(res, "civico")));
        const char* CAP = PQgetvalue(res, 0, PQfnumber(res, "cap"));
        const char* city = PQgetvalue(res, 0, PQfnumber(res, "citta"));
        const char* stato = PQgetvalue(res, 0, PQfnumber(res, "statoIND"));
        double totale = atof(PQgetvalue(res, 0, PQfnumber(res, "totale")));

        std::string ordine = "ID Ordine: " + std::to_string(ordineID) +
             " Mail Customer: " + mail + 
             " Data Richiesta: " + std::to_string(time) + 
             " Metodo Pagamento: " + paga +
             " Totale Ordine: " + std::to_string(totale) + "\n" + 
             " Da consegnare in: " + via + " " + std::to_string(civico) + ", " + city + " (CAP : " + CAP + "), " + stato + "\n";
	    send(clientSocket, ordine.c_str(), ordine.length(), 0);
        PQclear(res);
        //Poi recupera le informazioni dei prodotti al suo interno

        sprintf(comando, "SELECT pr.id, pr.descrizione, pr.nome, pr.prezzo, fr.nome AS nomeF, pn.quantita "
            "FROM prodotto pr, prodinord pn, fornitore fr WHERE pn.prodotto = pr.id "
            "AND pr.fornitore = fr.id AND pn.ordine = %d", ordineID);
        res = db.ExecSQLtuples(comando);
        rows = PQntuples(res); // Si presume ci siano prodotti nell'ordine
        std::string premessa = "PRODOTTI PRESENTI NELL'ORDINE:\n";
	    send(clientSocket, premessa.c_str(), premessa.length(), 0);
        for (int j = 0; j < rows; j++)
        {
            int IDProd = atoi(PQgetvalue(res, j, PQfnumber(res, "id")));
            const char* descrizione = PQgetvalue(res, j, PQfnumber(res, "descrizione"));
            double prezzo = atof(PQgetvalue(res, j, PQfnumber(res, "prezzo")));
            const char* nome = PQgetvalue(res, j, PQfnumber(res, "nome"));
            const char* fornitore = PQgetvalue(res, j, PQfnumber(res, "nomeF"));
            int quantita = atoi(PQgetvalue(res, j, PQfnumber(res, "quantita")));

            std::string prodotto = "\t" + std::to_string(j+1) + ") ID Prodotto: " + std::to_string(IDProd) +
                " Nome Prodotto: " + nome + 
                " Descrizione: " + descrizione + 
                " Fornitore: " + fornitore + 
                " Prezzo Prodotto: " + std::to_string(prezzo) + 
                " Quantità :" + std::to_string(quantita) + "\n";
	            send(clientSocket, prodotto.c_str(), prodotto.length(), 0);
        }
    }
    catch(...)
    {
        std::string errore = "C'è stato un problema con il database\n";
        send(clientSocket, errore.c_str(), errore.length(), 0);
    }
    PQclear(res); // Libera lo spazio occupato dai risultati della query
    return;
}
