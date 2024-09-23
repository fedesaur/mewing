#ifndef ROUTES_H
#define ROUTES_H

#pragma once
#include <utility> //Importa pair
#include <pistache/router.h>
#include "../../../lib/con2db/pgsql.h"
#include "../../../lib/con2redis/src/con2redis.h"
#include "../../../lib/funzioniAusiliarie.h"
#include "../../../entities/Prodotto.h"
#include "../metodi/autenticazione.h"
#include "../metodi/aggiungiFornito.h"
#include "../metodi/rimuoviFornito.h"
#include "../metodi/modificaFornito.h"
#include "../metodi/recuperaForniti.h"
#include "../metodi/modificaInfoF.h"
#include <pistache/http.h>
#include <pistache/endpoint.h>
#include <iostream>
#include <nlohmann/json.hpp>

#define HOSTNAME "localhost"
#define DB_PORT "5432"
#define USERNAMEP "producer"
#define PASSWORDP "producer"
#define DB_NAME "mewingdb"

void defineRoutes(Pistache::Rest::Router& router);
void autenticaFornitore(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
void creaFornitore(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
void aggiungiProdotto(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
void eliminaProdotto(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
void modificaProdotto(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
void modificaInfo(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
void getProdotti(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
int recuperaSupplierID(const std::string& email);

#endif // ROUTES_H
