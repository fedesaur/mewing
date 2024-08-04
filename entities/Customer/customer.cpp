#include "customer.h"


// Costruttore di Customer
Customer::Customer(
    int id,
    std::string nome,
    std::string cognome,
    std::string mail,
    int città
){
    ID = id;
    
    // La lunghezza del nome deve essere di massimo 20 caratteri
    assert(nome.length() > 0 && nome.length() <= 20);
    Nome = nome;

    // La lunghezza del cognome deve essere di massimo 20 caratteri
    assert(cognome.length() > 0 && cognome.length() <= 20);
    Cognome = cognome;

    //La lunghezza della mail deve essere di massimo 50 caratteri
    assert(mail.length() > 0 && mail.length() <= 50);
    Mail = mail;

    Abita = città;   
}
void AggiungiIndirizzo(
    Con2DB db1,
    std::string via,
    int civico,
    std::string cap,
    std::string city,
    std::string stato
){
    // Res immagazzina le risposte del database ed eventuali errori
    PGResult *res
    
    // Effettuati controlli sui parametri per fare in modo che rispettino i limiti richiesti
    assert(via.length() > 0 && via.length() <= 100);
    assert(civico >= 0);
    assert(cap.length() == 5);
    assert(city.length() > 0 && city.length() <= 30);
    assert(stato.length() > 0 && stato.length() <= 30);

    /* sprintf si occupa di creare una stringa, dandogli un format e dei parametri
    sprintf(destinazione, formattazione, parametri)
    ESEMPIO:
    sprintf(stringa, "%d + %d fa %d, facile!", 10, 12, 22)
    stringa sarà uguale a "10 + 12 fa 22, facile!"
    */
    sprintf(comando,
    "INSERT INTO Indirizzo (via, civico, cap, citta, stato)
        VALUES (\'%s\', \'%d\', \'%s\', \'%s\', \'%s\') 
        ON CONFLICT DO NOTHING",
	    via, civico, cap, city, stato);
    res = db1.ExecSQLcmd(comando);
    PQclear(res);
}