#ifndef ROUTES_H
#define ROUTES_H

#pragma once
#include <pistache/router.h>
#include "../../../lib/con2db/pgsql.h"
#include "../../../lib/con2redis/src/con2redis.h"
#include "../../../entities/Indirizzo.h"
#include "../../../entities/Ordine.h"
#include "../metodi/autenticazione.h"
#include <pistache/http.h>
#include <pistache/endpoint.h>
#include <iostream>

#define HOSTNAME "localhost"
#define DB_PORT "5432"
#define USERNAMEP "producer"
#define PASSWORDP "producer"
#define DB_NAME "mewingdb"

void defineRoutes(Pistache::Rest::Router& router);

void autenticaFornitore(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
#endif // ROUTES_H
