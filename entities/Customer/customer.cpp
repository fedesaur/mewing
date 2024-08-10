#include "customer.h"

#define STREAM_NAME "Customer_Stream"

// Costruttore di Customer
Customer::Customer(
    int id,
    std::string nome,
    std::string cognome,
    std::string mail,
    int città
){
     // Effettuati controlli sui parametri per fare in modo che rispettino i limiti richiesti
    assert(nome.length() > 0 && nome.length() <= 20);
    assert(cognome.length() > 0 && cognome.length() <= 20);
    assert(mail.length() > 0 && mail.length() <= 50);

    ID = id;
    Nome = nome;
    Cognome = cognome;
    Mail = mail;
    Abita = città;   
}
void AggiungiIndirizzo(
    std::string via,
    int civico,
    std::string cap,
    std::string city,
    std::string stato
){
    /* Effettua la connessione al database
    (Conviene farla in un posto unico come un main)
    */ 
    Con2DB db1("Simone",
    "4032",
    "Simy8000",
    "12345",
    "SUPERDB");
    redisContext *c2r; // c2r contiene le info sul contesto
    redisReply *reply; // reply contiene le risposte da Redis
    PGResult *res; // res immagazzina le risposte del database ed eventuali errori
    char comando[1000]; // comando conserva le query da eseguire nel database
    
    // Effettuati controlli sui parametri per fare in modo che rispettino i limiti richiesti
    assert(via.length() > 0 && via.length() <= 100);
    assert(civico >= 0);
    assert(cap.length() == 5);
    assert(city.length() > 0 && city.length() <= 30);
    assert(stato.length() > 0 && stato.length() <= 30);

    /* Effettua la connessione a Redis
    (Anche qui, controllare se conviene metterlo in un main)
    */
    c2r = redisConnect("localhost", 6379);

    // Crea uno stream di nome STREAM_NAME (in questo caso Customer_Stream)
    initStreams(c2r, STREAM_NAME);
    
    /* sprintf si occupa di creare una stringa, dandogli un format e dei parametri
    sprintf(destinazione, formattazione, parametri)
    ESEMPIO:
    sprintf(stringa, "%d + %d fa %d, facile!", 10, 12, 22)
    stringa sarà uguale a "10 + 12 fa 22, facile!"
    */
    sprintf(comando,
    "INSERT INTO Indirizzo (via, civico, cap, citta, stato)
        VALUES (\'%s\', %d, \'%s\', \'%s\', \'%s\') 
        ON CONFLICT DO NOTHING",
	    via, civico, cap, city, stato);
    printf("%s", comando);
    //res = db1.ExecSQLcmd(comando);
    PQclear(res);
}