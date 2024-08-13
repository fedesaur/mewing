#!/bin/sh
sudo -u postgres psql postgres -f parametri.sql -f table.sql -f trigger.sql -f function.sql -f fill.sql 
