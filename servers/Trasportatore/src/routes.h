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
#include "../metodi/prendiOrdine.h"
#include "../metodi/registroOrdini.h"
#include "../metodi/ricercaOrdini.h"
#include <pistache/http.h>
#include <pistache/endpoint.h>
#include <iostream>
#include <nlohmann/json.hpp>

#define WRITE_STREAM "CourierW"
#define READ_STREAM "CourierR"
#define REDIS_IP "localhost"
#define REDIS_PORT 6379
#define HOSTNAME "localhost"
#define DB_PORT "5432"
#define USERNAME_TRAS "courier"
#define PASSWORD_TRAS "courier"
#define DB_NAME "mewingdb"

void defineRoutes(Pistache::Rest::Router& router);

void autenticaTrasportatore(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
void creaTrasportatore(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
void getOrdini(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
void accettaOrdine(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
int recuperaCourierID(std::string PIVA);
#endif // ROUTES_H
