#ifndef ROUTES_H
#define ROUTES_H

#pragma once
#include <pistache/router.h>
#include "../../../lib/con2db/pgsql.h"
#include "../../../lib/con2redis/src/con2redis.h"
#include "../../../entities/Indirizzo.h"
#include "../../../entities/Ordine.h"
#include "../metodi/autenticazione.h"
#include "../metodi/gestisciCorrieri.h"
#include "../metodi/registroOrdini.h"
#include "../metodi/ricercaOrdini.h"
#include <pistache/http.h>
#include <pistache/endpoint.h>
#include <iostream>

#define HOSTNAME "localhost"
#define DB_PORT "5432"
#define USERNAMEP "producer"
#define PASSWORDP "producer"
#define DB_NAME "mewingdb"

void defineRoutes(Pistache::Rest::Router& router);

void autenticaTrasportatore(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
void getOrdini(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
void addProdottoToCarrello(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
int recuperaTrasporterID(std::string IVA);
#endif // ROUTES_H
