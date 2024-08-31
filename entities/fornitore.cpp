#include "fornitore.h"
#include <string>
#include <cassert>
Fornitore::Fornitore(
    int id,
    std::string nome,
    std::string iva,
    std::string mail,
    std::string tel,
    int sede
){
    ID = id;

    // La lunghezza del nome deve essere di massimo 100 caratteri
    assert(nome.length() > 0 && nome.length() <= 100);
    Nome = nome;

    // La lunghezza dell'IVA è di 11 caratteri
    assert(iva.length() == 11);
    IVA = iva;

    //La lunghezza della mail deve essere di massimo 50 caratteri
    assert(mail.length() > 0 && mail.length() <= 50);
    Mail = mail;
    
    Telefono = tel;
    Sede = sede;
}