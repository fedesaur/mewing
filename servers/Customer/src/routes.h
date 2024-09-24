#ifndef ROUTES_TRASPORTATORE_H
#define ROUTES_TRASPORTATORE_H

#pragma once
#include <pistache/router.h>
#include "../../../lib/con2db/pgsql.h"
#include "../../../lib/con2redis/src/con2redis.h"
#include "../../../entities/Indirizzo.h"
#include "../metodi/gestisciCarrello.h"
#include "../metodi/autenticazione.h"
#include "../metodi/recuperaProdotti.h"
#include "../metodi/recuperaCarrello.h"
#include "../metodi/gestisciCarrello.h"
#include <pistache/http.h>
#include <pistache/endpoint.h>
#include <pistache/router.h>
#include "routes.h"
#include <iostream>

void defineRoutes(Pistache::Rest::Router& router);

void autenticaCustomer(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
void creaCustomer(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
void modificaInfo(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
void getIndirizzi(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response)
void getProdotti(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
void addProdottoToCarrello(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
int recuperaCustomerID(std::string email);
void getCarrello(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
void ordina(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);

#endif // ROUTES_H
