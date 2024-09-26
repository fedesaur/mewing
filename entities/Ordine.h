#ifndef ORDINE_H
#define ORDINE_H

struct Ordine{
    int ID;
    const char* MailCustomer;
    const char* DataRichiesta;
    const char* DataConsegna;
    const char* Stato;
    const char* Pagamento;
    int Indirizzo;
    double Totale;
};
#endif
