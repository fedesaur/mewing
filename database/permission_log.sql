\c :log_dbname postgres

GRANT ALL PRIVILEGES ON DATABASE :log_dbname to :admin;
GRANT CONNECT ON DATABASE :log_dbname TO :handler;

ALTER TABLE cliente OWNER to :admin;
ALTER TABLE comunicazione OWNER TO :admin;

GRANT ALL ON SCHEMA public TO :admin;
GRANT ALL PROVILEGES ON ALL TABLES IN SCHEMA public to :admin;

GRANT USAGE, SELECT ON ALL SEQUENCES IN SCHEMA public TO :admin;

GRANT SELECT, INSERT, UPDATE ON cliente TO :handler;