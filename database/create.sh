#!/bin/sh
sudo -u postgres psql postgres -f parametri.sql -f table.sql -f trigger.sql -f permission.sql -f setup.sql -f setup_log.sql -f table_log.sql -f function.sql -f fill.sql 
