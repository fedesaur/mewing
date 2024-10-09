#ifndef ROUTES_H
#define ROUTES_H

#pragma once
#include <pistache/router.h>
#include "../../../lib/con2db/pgsql.h"
#include "../../../lib/con2redis/src/con2redis.h"
#include "../../../lib/funzioniAusiliarie.h"
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
#include "../metodi/ordiniCorrenti.h"
#include "../metodi/prodottiOrdine.h"

#include <pistache/http.h>
#include <pistache/endpoint.h>
#include <iostream>
#include <nlohmann/json.hpp>

#define WRITE_STREAM "CourierW"
#define READ_STREAM "CourierR"
#define REDIS_IP "localhost"
#define REDIS_PORT 6379

#define USERNAME_HANDLER "handler"
#define PASSWORD_HANDLER "handler"
#define LOG_DB_NAME "mewinglogdb"

#define HOSTNAME "localhost"
#define DB_PORT "5432"
#define USERNAME_TRAS "courier"
#define PASSWORD_TRAS "courier"
#define DB_NAME "mewingdb"
#define TEMPO_LIMITE 1 // Si considera come tempo limite un secondo

void defineRoutes(Pistache::Rest::Router& router);

void autenticaTrasportatore(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
void creaTrasportatore(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
void getOrdini(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
void getCorrenti(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
void getCorrieri(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
void getRegistrati(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
void accettaOrdine(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
void putCorriere(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
void deleteCorriere(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
void consegna(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
int recuperaCourierID(std::string IVA);

//Operazione con il Database dei Log
int inserimentoOperazione(int customerID, const char* operazione);
bool successoOperazione(int logID);
bool fallimentoOperazione(int logID);

#endif // ROUTES_H
