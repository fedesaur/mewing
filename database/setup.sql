CREATE DATABASE IF NOT EXISTS :dbname;

\c :dbname postgres

REASSIGN OWNED BY :admin TO postgres;

REVOKE ALL PRIVILEGES ON ALL TABLES IN SCHEMA public FROM :admin;
REVOKE ALL PRIVILEGES ON ALL SEQUENCES IN SCHEMA public FROM :admin;
REVOKE ALL PRIVILEGES ON ALL FUNCTION IN SCHEMA public FROM :admin;

DROP OWNED BY :admin;
DROP USER IF EXISTS :admin;

CREATE USER :admin WITH ENCRYPTED PASSWORD 'admin';

\c :dbname postgres;

REASSIGN OWNED BY customer TO postgres;
REVOKE ALL PRIVILEGES ON ALL TABLES IN SCHEMA public FROM customer;
REVOKE ALL PRIVILEGES ON ALL SEQUENCES IN SCHEMA public FROM customer;
REVOKE ALL PRIVILEGES ON ALL FUNCTION IN SCHEMA public FROM customer;

DROP OWNED BY :customer;
DROP OWNER IF EXISTS :customer;

CREATE USER :customer WITH ENCRYPTED PASSWORD 'customer';

\c :dbname postgres

REASSIGN OWNED BY :producer TO postgres;

REVOKE ALL PRIVILEGES ON ALL TABLES IN SCHEMA public FROM :produced;
REVOKE ALL PRIVILEGES ON ALL SEQUENCES IN SCHEMA public FROM :producer;
REVOKE ALL PRIVILEGES ON ALL FUNCTION IN SCHEMA public FROM :producer;

DROP OWNED BY :producer;
DROP OWNER IF EXISTS :producer;

CREATE USER :producer WITH ENCRYPTED PASSWORD 'producer';

\c :dbname postgres;

REASSIGN OWNED BY :courier TO postgres;
REVOKE ALL PRIVILEGES ON ALL TABLES IN SCHEMA public FROM :courier;
REVOKE ALL PRIVILEGES ON ALL SEQUENCES IN SCHEMA public FROM :courier;
REVOKE ALL PRIVILEGES ON ALL FUNCTION IN SCHEMA public FROM :courier;

DROP OWNED BY :courier;
DROP OWNER IF EXISTS :courier;

CREATE USER :courier WITH ENCRYPTED PASSWORD 'courier';




