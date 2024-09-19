#ifndef ORDINE_H
#define ORDINE_H

struct Ordine{
    int ID;
    const char* MailCustomer;
    time_t DataRichiesta;
    time_t DataConsegna;
    const char* Stato;
    const char* Pagamento;
    int Indirizzo;
    double Totale;
};
#endif
