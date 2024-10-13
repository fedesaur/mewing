#!/bin/sh

sudo -u postgres psql postgres -f parametri.sql -f setup.sql -f table.sql -f permission.sql -f trigger.sql -f setup_log.sql -f table_log.sql -f trigger_log.sql -f function.sql -f fill.sql -f permission_log.sql

sudo sed -i "s/^max_connections=.*/max_connections=1000/" /etc/postgresql/16/main/postgresql.conf

sudo systemctl restart postgresql
