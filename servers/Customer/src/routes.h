#ifndef ROUTES_H
#define ROUTES_H

#pragma once
#include <pistache/router.h>
#include "../../../lib/con2db/pgsql.h"
#include "../../../lib/con2redis/src/con2redis.h"

void defineRoutes(Pistache::Rest::Router& router);

void authenticateUser(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
void modificaNomeHttp(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);

#endif // ROUTES_H
