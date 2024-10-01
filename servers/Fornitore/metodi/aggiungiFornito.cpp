#include "aggiungiFornito.h"

using namespace std::chrono_literals;
using Clock = std::chrono::system_clock;

bool aggiungiFornito(int supplierID, const char* nomeProdotto, const char* descrizioneProdotto, double prezzoProdotto)
{
    PGresult *res;
    char comando[1000];
	Con2DB db(HOSTNAME, DB_PORT, USERNAMEP, PASSWORDP, DB_NAME);
    auto timeLimit = Clock::now() + 5ms;
    auto start = std::chrono::high_resolution_clock::now();
    while (Clock::now() < timeLimit)
    {
        try
        {
            sprintf(comando, "INSERT INTO prodotto(descrizione, prezzo, nome, fornitore) VALUES('%s', %f, '%s', %d)",
            descrizioneProdotto, prezzoProdotto, nomeProdotto, supplierID);
            res = db.ExecSQLcmd(comando);
            PQclear(res);
            auto finish = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double, std::milli> elapsed = finish - start;
            std::cout << "Elapsed Time: " << elapsed.count() << " seconds" << std::endl;
            return true;
        }
        catch(...)
        {
            PQclear(res);
            return false;
        }
    }
    std::cout << "Timer scattato" << std::endl;
    return false;


}
