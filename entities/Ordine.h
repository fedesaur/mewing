#ifndef ORDINE_H
#define ORDINE_H

enum StatoOrdine {
    ANNULLATO,
    PENDENTE,
    ACCETTATO
};

struct Ordine{
    int ID;
    int CustomerID;
    time_t DataRichiesta;
    StatoOrdine Stato;
    int Pagamento;
    int Indirizzo;
    double Totale;
};
#endif