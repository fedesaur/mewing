#ifndef ROUTES_H
#define ROUTES_H

#pragma once
#include <pistache/router.h>
#include "../../../lib/con2db/pgsql.h"
#include "../../../lib/con2redis/src/con2redis.h"
#include "../metodi/aggiungiCarrello.h"

void defineRoutes(Pistache::Rest::Router& router);

void authenticateUser(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
void modificaNomeHttp(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
void getProdotti(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
void addProdottoToCarrello(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
int recuperaCustomerID(const std::string& email);

#endif // ROUTES_H
