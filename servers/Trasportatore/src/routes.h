#ifndef ROUTES_H
#define ROUTES_H

#pragma once
#include <pistache/router.h>
#include "../../../lib/con2db/pgsql.h"
#include "../../../lib/con2redis/src/con2redis.h"
// Entità utilizzate
#include "../../../entities/Indirizzo.h"
#include "../../../entities/Ordine.h"
#include "../../../entities/Corriere.h"
#include "../../../entities/Prodotto.h"
// Metodi per l'autenticazione
#include "../metodi/autenticazione.h"
// Metodi per i corrieri
#include "../metodi/recuperaCorrieri.h"
#include "../metodi/registraCorriere.h"
#include "../metodi/rimuoviCorriere.h"
// Metodi per gli ordini
#include "../metodi/prendiOrdine.h"
#include "../metodi/registroOrdini.h"
#include "../metodi/ricercaOrdini.h"
#include "../metodi/consegnaOrdine.h"
#include "../metodi/dettagliOrdine.h"

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
void getCorrieri(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
void getDettagli(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
void accettaOrdine(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
void putCorriere(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
void deleteCorriere(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
void consegna(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
int recuperaCourierID(std::string IVA);

#endif // ROUTES_H
