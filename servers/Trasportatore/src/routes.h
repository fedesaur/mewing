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

#define HOSTNAME "localhost"
#define DB_PORT "5432"
#define USERNAME_TRAS "courier"
#define PASSWORD_TRAS "courier"
#define DB_NAME "mewingdb"

void defineRoutes(Pistache::Rest::Router& router);

void autenticaTrasportatore(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
void getOrdini(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
void addProdottoToCarrello(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
int recuperaTrasporterID(std::string IVA);
void getCarrello(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
void ordina(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);

#endif // ROUTES_H
