\c :log_dbname postgres

GRANT ALL PRIVILEGES ON DATABASE :log_dbname to :admin;
GRANT CONNECT ON DATABASE :log_dbname TO :handler;

ALTER TABLE cliente OWNER to :admin;

GRANT ALL ON SCHEMA public TO :admin;
GRANT ALL PRIVILEGES ON ALL TABLES IN SCHEMA public to :admin;

GRANT USAGE, SELECT ON ALL SEQUENCES IN SCHEMA public TO :admin;
GRANT USAGE, SELECT ON ALL SEQUENCES IN SCHEMA public TO :handler;

GRANT SELECT, INSERT, UPDATE ON cliente TO :handler;