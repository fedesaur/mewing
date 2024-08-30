#include "ordine.h"

// Costruttore di Ordine
Ordine::Ordine(
            int id,
            int customer,
            time_t data,
            int pagamento,
            int indirizzo,
            float totale
){
    ID = id;
    CustomerID = customer;
    DataRichiesta = data;
    Stato = PENDENTE; // Di default, è Pendente
    Pagamento = pagamento;
    Indirizzo = indirizzo;
    Totale = totale;
}