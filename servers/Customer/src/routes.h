#ifndef ROUTES_TRASPORTATORE_H
#define ROUTES_TRASPORTATORE_H

#pragma once
#include "../../../lib/con2db/pgsql.h"
#include "../../../lib/con2redis/src/con2redis.h"
#include "../../../entities/Indirizzo.h"
#include "../../../entities/Prodotto.h"
#include "../../../entities/Ordine.h"
#include "../metodi/aggiungiIndirizzo.h"
#include "../metodi/aggiungiCarrello.h"
#include "../metodi/rimuoviIndirizzo.h"
#include "../metodi/annullaOrdine.h"
#include "../metodi/modificaNome.h"
#include "../metodi/rimuoviCarrello.h"
#include "../metodi/recuperaIndirizzi.h"
#include "../metodi/autenticazione.h"
#include "../metodi/recuperaProdotti.h"
#include "../metodi/recuperaCarrello.h"
#include "../metodi/recuperaOrdini.h"
#include "../metodi/effettuaOrdine.h"
#include <pistache/http.h>
#include <pistache/endpoint.h>
#include <pistache/router.h>
#include "routes.h"
#include <iostream>
#include <nlohmann/json.hpp>

#define HOSTNAME "localhost"
#define DB_PORT "5432"

#define USERNAME_HANDLER "handler"
#define PASSWORD_HANDLER "handler"
#define LOG_DB_NAME "mewinglogdb"

#define USERNAME "admin"
#define PASSWORD "admin"
#define DB_NAME "mewingdb"

void defineRoutes(Pistache::Rest::Router& router);

void autenticaCustomer(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
void creaCustomer(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
void modificaInfo(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
void getOrdini(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
void getIndirizzi(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
void getProdotti(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
void addProdottoToCarrello(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
void addIndirizzo(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
void removeCarrello(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
void getCarrello(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
void ordina(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
void annullaOrd(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
void removeIndirizzo(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);

int recuperaCustomerID(std::string email); // Funzioni ausiliaria per il recupero dell'ID del customer dal DB

//Operazione con il Database dei Log
int inserimentoOperazione(int customerID, const char* operazione);
bool successoOperazione(int logID);
bool fallimentoOperazione(int logID);

#endif // ROUTES_H
