\c mewingDB postgres

GRANT ALL PRIVILEGES ON DATABASE mewingDB to :admin;
GRANT CONNECT ON DATABASE mewingDB TO :courier;
GRANT CONNECT ON DATABASE mewingDB TO :producer;
GRANT CONNECT ON DATABASE mewingDB TO :customer;

ALTER TABLE carrello OWNER TO :admin;
ALTER TABLE custadd OWNER TO :admin;
ALTER TABLE customers OWNER TO :admin;
ALTER TABLE fornitore OWNER TO :admin;
ALTER TABLE indirizzo OWNER TO :admin;
ALTER TABLE inwish OWNER TO :admin;
ALTER TABLE metpag OWNER TO :admin;
ALTER TABLE ordine OWNER TO :admin;
ALTER TABLE ordineconse OWNER TO :admin;
ALTER TABLE prodincart OWNER TO :admin;
ALTER TABLE prodinord OWNER TO :admin;
ALTER TABLE prodmet OWNER TO :admin;
ALTER TABLE prodotto OWNER TO :admin;
ALTER TABLE transord OWNER TO :admin;
ALTER TABLE trasportatore OWNER TO :admin;
ALTER TABLE wishlist OWNER TO :admin;

GRANT ALL ON SCHEMA public TO :admin;
GRANT ALL PRIVILEGES ON ALL TABLES IN SCHEMA public TO :admin;

GRANT USAGE, SELECT ON ALL SEQUENCES IN SCHEMA public TO :admin;
GRANT USAGE, SELECT ON ALL SEQUENCES IN SCHEMA public TO :customer;
GRANT USAGE, SELECT ON ALL SEQUENCES IN SCHEMA public TO :producer;
GRANT USAGE, SELECT ON ALL SEQUENCES IN SCHEMA public TO :courier;


GRANT SELECT ON ordine TO :courier;
GRANT INSERT, UPDATE, SELECT on ordineconse to :courier;
GRANT INSERT, UPDATE, SELECT on indirizzo to :courier;

GRANT INSERT, UPDATE, SELECT, DELETE on prodotto to :producer;
GRANT INSERT, UPDATE, SELECT, DELETE on fornitore to :producer;
GRANT INSERT, UPDATE, SELECT, DELETE on prodmet to :producer;
GRANT INSERT, UPDATE, SELECT on indirizzo to :producer;
GRANT SELECT ON ordine to :producer;
GRANT SELECT ON prodinord to :producer;


GRANT INSERT, UPDATE, SELECT, DELETE on carrello to :customer;
GRANT INSERT, UPDATE, SELECT, DELETE on custadd to :customer;
GRANT INSERT, UPDATE, SELECT, DELETE on indirizzo to :customer;
GRANT INSERT, UPDATE, SELECT, DELETE on inwish to :customer;
GRANT INSERT, UPDATE, SELECT, DELETE on metpag to :customer;
GRANT INSERT, UPDATE, DELETE on ordine to :customer;
GRANT INSERT, UPDATE, DELETE on prodincart to :customer;
GRANT INSERT, UPDATE, DELETE, SELECT on wishlist to :customer;



