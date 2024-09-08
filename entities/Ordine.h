#ifndef ORDINE_H
#define ORDINE_H

struct Ordine{
    int ID;
    const char* MailCustomer;
    time_t DataRichiesta;
    double Stato;
    const char* Pagamento;
    int Indirizzo;
    double Totale;
};
#endif
