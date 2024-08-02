#include "customer.h"
#include <string>

Customer::Customer(
    int id,
    std::string nome,
    std::string cognome,
    std::string mail,
    int città
){
    ID = id;
    Nome = nome;
    Cognome = cognome;
    Mail = mail;
    Abita = città;   
}