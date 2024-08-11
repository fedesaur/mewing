#!/bin/sh
sudo -u postgres psql postgres -f funzioni.sql -f tabelle.sql  -f trigger.sql  -f parametri.sql
