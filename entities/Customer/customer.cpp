#include "customer.h"
#include <string>
#include <cassert>

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