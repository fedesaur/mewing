#include <time.h>

enum StatoOrdine {
    ANNULLATO,
    PENDENTE,
    ACCETTATO
};

class Ordine{
    public:
        int ID = 0;
        int CustomerID = 0;
        time_t DataRichiesta = NULL;
        StatoOrdine Stato = PENDENTE;
        int Pagamento = 0;
        int Indirizzo = 0;
        float Totale = 0;

        Ordine(
            int id,
            int customer,
            time_t data,
            int pagamento,
            int indirizzo,
            float totale
        );
};