#ifndef ROUTES_H
#define ROUTES_H

#pragma once
#include <pistache/router.h>

void defineRoutes(Pistache::Rest::Router& router);

void authenticateUser(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
void modificaNomeHttp(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);

#endif // ROUTES_H
