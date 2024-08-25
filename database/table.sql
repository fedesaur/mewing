CREATE TYPE dim AS
(
    lunghezza pos,
    larghezza pos,
    altezza pos
);
CREATE TYPE statoordine AS ENUM
    ('annullato', 'pendente', 'accettato');

CREATE TYPE tipometpag AS ENUM
    ('Virtuale', 'contante', 'buono', 'carta prepagata', 'carta di credito','bancomat');




CREATE TABLE IF NOT EXISTS buono
(
    id integer NOT NULL,
    scadenza timestamp without time zone NOT NULL,
    valore pos NOT NULL,
    CONSTRAINT buono_id_fkey FOREIGN KEY (id)
        REFERENCES metpag (id) MATCH SIMPLE
        ON UPDATE NO ACTION
        ON DELETE NO ACTION
)



CREATE TABLE IF NOT EXISTS carrello
(
    customer integer NOT NULL,
    totale numeric NOT NULL DEFAULT 0,
    CONSTRAINT carrello_customer_key UNIQUE (customer),
    CONSTRAINT carrello_customer_fkey FOREIGN KEY (customer)
        REFERENCES customers (id) MATCH SIMPLE
        ON UPDATE NO ACTION
        ON DELETE NO ACTION
)



CREATE TABLE IF NOT EXISTS consegna
(
    ordine integer NOT NULL,
    corriere integer NOT NULL,
    CONSTRAINT consegna_pkey PRIMARY KEY (ordine, corriere),
    CONSTRAINT consegna_corriere_fkey FOREIGN KEY (corriere)
        REFERENCES corriere (id) MATCH SIMPLE
        ON UPDATE NO ACTION
        ON DELETE NO ACTION,
    CONSTRAINT consegna_ordine_fkey FOREIGN KEY (ordine)
        REFERENCES ordine (id) MATCH SIMPLE
        ON UPDATE NO ACTION
        ON DELETE NO ACTION
)



-- Trigger: same_trans

-- DROP TRIGGER IF EXISTS same_trans ON consegna;

CREATE OR REPLACE TRIGGER same_trans
    BEFORE INSERT
    ON consegna
    FOR EACH ROW
    EXECUTE FUNCTION check_del();


CREATE TABLE IF NOT EXISTS corriere
(
    id integer NOT NULL DEFAULT nextval('corriere_id_seq'::regclass),
    azienda integer NOT NULL,
    nome character varying(20) COLLATE pg_catalog."default" NOT NULL,
    cognome character varying(20) COLLATE pg_catalog."default" NOT NULL,
    CONSTRAINT corriere_pkey PRIMARY KEY (id),
    CONSTRAINT corriere_azienda_fkey FOREIGN KEY (azienda)
        REFERENCES trasportatore (id) MATCH SIMPLE
        ON UPDATE NO ACTION
        ON DELETE NO ACTION
)



CREATE TABLE IF NOT EXISTS custadd
(
    customer integer NOT NULL,
    addr integer NOT NULL,
    CONSTRAINT custadd_pkey PRIMARY KEY (customer, addr),
    CONSTRAINT custadd_addr_fkey FOREIGN KEY (addr)
        REFERENCES indirizzo (id) MATCH SIMPLE
        ON UPDATE NO ACTION
        ON DELETE NO ACTION,
    CONSTRAINT custadd_customer_fkey FOREIGN KEY (customer)
        REFERENCES customers (id) MATCH SIMPLE
        ON UPDATE NO ACTION
        ON DELETE NO ACTION
)



CREATE TABLE IF NOT EXISTS customers
(
    id integer NOT NULL DEFAULT nextval('customer_id_seq'::regclass),
    nome character varying(20) COLLATE pg_catalog."default",
    cognome character varying(20) COLLATE pg_catalog."default",
    mail character varying(50) COLLATE pg_catalog."default",
    abita integer,
    CONSTRAINT customers_pkey PRIMARY KEY (id),
    CONSTRAINT customers_abita_fkey FOREIGN KEY (abita)
        REFERENCES indirizzo (id) MATCH SIMPLE
        ON UPDATE NO ACTION
        ON DELETE NO ACTION
)

CREATE TABLE IF NOT EXISTS fornitore
(
    id integer NOT NULL DEFAULT nextval('fornitore_id_seq'::regclass),
    nome character varying(100) COLLATE pg_catalog."default" NOT NULL,
    piva character varying(11) COLLATE pg_catalog."default" NOT NULL,
    mail character varying(50) COLLATE pg_catalog."default" NOT NULL,
    telefono integer,
    sede integer,
    CONSTRAINT fornitore_pkey PRIMARY KEY (id),
    CONSTRAINT unique_piva UNIQUE (piva),
    CONSTRAINT fornitore_sede_fkey FOREIGN KEY (sede)
        REFERENCES indirizzo (id) MATCH SIMPLE
        ON UPDATE NO ACTION
        ON DELETE NO ACTION,
    CONSTRAINT lengpiva CHECK (length(piva::text) = 11)
)

CREATE TABLE IF NOT EXISTS indirizzo
(
    id integer NOT NULL DEFAULT nextval('addr_id_sequence'::regclass),
    via character varying(100) COLLATE pg_catalog."default" NOT NULL,
    civico pos,
    cap character varying(5) COLLATE pg_catalog."default" NOT NULL,
    citta character varying(30) COLLATE pg_catalog."default" NOT NULL,
    stato character varying(30) COLLATE pg_catalog."default" NOT NULL,
    CONSTRAINT indirizzo_pkey PRIMARY KEY (id)
)

CREATE TABLE IF NOT EXISTS inwish
(
    list integer NOT NULL,
    prodotto integer NOT NULL,
    CONSTRAINT inwish_pkey PRIMARY KEY (list, prodotto),
    CONSTRAINT inwish_list_fkey FOREIGN KEY (list)
        REFERENCES wishlist (id) MATCH SIMPLE
        ON UPDATE NO ACTION
        ON DELETE NO ACTION,
    CONSTRAINT inwish_prodotto_fkey FOREIGN KEY (prodotto)
        REFERENCES prodotto (id) MATCH SIMPLE
        ON UPDATE NO ACTION
        ON DELETE NO ACTION
)

CREATE TABLE IF NOT EXISTS metpag
(
    id integer NOT NULL DEFAULT nextval('metpag_id_seq'::regclass),
    nome character varying(50) COLLATE pg_catalog."default" NOT NULL,
    tipo tipometpag NOT NULL,
    Customer integer,
	foreign key (customer) reference customers(id),

    CONSTRAINT metpag_pkey PRIMARY KEY (id),
    
)


CREATE TABLE IF NOT EXISTS ordine
(
    id integer NOT NULL DEFAULT nextval('ordine_id_seq'::regclass),
    customer integer NOT NULL,
    datarich timestamp without time zone NOT NULL,
    stato statoordine NOT NULL DEFAULT 'pendente'::statoordine,
    pagamento tipometpag,
    indirizzo integer NOT NULL,
    totale numeric NOT NULL,
    CONSTRAINT ordine_pkey PRIMARY KEY (id),
    CONSTRAINT "indCust" FOREIGN KEY (indirizzo)
        REFERENCES indirizzo (id) MATCH SIMPLE
        ON UPDATE NO ACTION
        ON DELETE NO ACTION
        NOT VALID,
    CONSTRAINT "metPag" FOREIGN KEY (pagamento)
        REFERENCES metpag (id) MATCH SIMPLE
        ON UPDATE NO ACTION
        ON DELETE NO ACTION
        NOT VALID,
    CONSTRAINT ordine_customer_fkey FOREIGN KEY (customer)
        REFERENCES customers (id) MATCH SIMPLE
        ON UPDATE NO ACTION
        ON DELETE NO ACTION
)
CREATE OR REPLACE TRIGGER same_addr
    BEFORE INSERT
    ON ordine
    FOR EACH ROW
    EXECUTE FUNCTION check_addr();



CREATE TABLE IF NOT EXISTS ordineconse
(
    id integer NOT NULL,
    datacons timestamp without time zone NOT NULL,
    CONSTRAINT ordineconse_pkey PRIMARY KEY (id),
    CONSTRAINT ordineconse_id_fkey FOREIGN KEY (id)
        REFERENCES ordine (id) MATCH SIMPLE
        ON UPDATE NO ACTION
        ON DELETE NO ACTION
)


CREATE TABLE IF NOT EXISTS prodincart
(
    carrello integer NOT NULL,
    prodotto integer NOT NULL,
    quantita integer,
    CONSTRAINT prodincart_pkey PRIMARY KEY (carrello, prodotto),
    CONSTRAINT prodincart_carrello_fkey FOREIGN KEY (carrello)
        REFERENCES carrello (customer) MATCH SIMPLE
        ON UPDATE NO ACTION
        ON DELETE NO ACTION,
    CONSTRAINT prodincart_prodotto_fkey FOREIGN KEY (prodotto)
        REFERENCES prodotto (id) MATCH SIMPLE
        ON UPDATE NO ACTION
        ON DELETE NO ACTION
)
CREATE OR REPLACE TRIGGER buy_price
    BEFORE INSERT
    ON prodincart
    FOR EACH ROW
    EXECUTE FUNCTION tot_cart();



CREATE TABLE IF NOT EXISTS prodinord
(
    prodotto integer NOT NULL,
    ordine integer NOT NULL,
    quantita integer NOT NULL DEFAULT 1,
    CONSTRAINT prodinord_pkey PRIMARY KEY (ordine, prodotto),
    CONSTRAINT prodinord_ordine_fkey FOREIGN KEY (ordine)
        REFERENCES ordine (id) MATCH SIMPLE
        ON UPDATE NO ACTION
        ON DELETE NO ACTION,
    CONSTRAINT prodinord_prodotto_fkey FOREIGN KEY (prodotto)
        REFERENCES prodotto (id) MATCH SIMPLE
        ON UPDATE NO ACTION
        ON DELETE NO ACTION
)
CREATE OR REPLACE TRIGGER same_meth
    BEFORE INSERT
    ON prodinord
    FOR EACH ROW
    EXECUTE FUNCTION check_meth();



CREATE TABLE IF NOT EXISTS prodmet
(
    prodotto integer NOT NULL,
    metodo tipometpag NOT NULL,
    CONSTRAINT prodmet_pkey PRIMARY KEY (prodotto, metodo),
    
    CONSTRAINT prodmet_prodotto_fkey FOREIGN KEY (prodotto)
        REFERENCES prodotto (id) MATCH SIMPLE
        ON UPDATE NO ACTION
        ON DELETE NO ACTION
)

CREATE TABLE IF NOT EXISTS prodotto
(
    id integer NOT NULL DEFAULT nextval('prodotto_id_seq'::regclass),
    descrizione text COLLATE pg_catalog."default" NOT NULL,
    prezzo double precision NOT NULL,
    dimensione dim NOT NULL,
    fornitore integer NOT NULL,
    nome character varying(100) COLLATE pg_catalog."default" NOT NULL,
    CONSTRAINT prodotto_pkey PRIMARY KEY (id),
    CONSTRAINT prodotto_fornitore_fkey FOREIGN KEY (fornitore)
        REFERENCES fornitore (id) MATCH SIMPLE
        ON UPDATE NO ACTION
        ON DELETE NO ACTION
)

CREATE TABLE IF NOT EXISTS transord
(
    ordine integer NOT NULL,
    trasportatore integer NOT NULL,
    CONSTRAINT "transOrd_pkey" PRIMARY KEY (ordine, trasportatore),
    CONSTRAINT "transOrd_ordine_fkey" FOREIGN KEY (ordine)
        REFERENCES ordine (id) MATCH SIMPLE
        ON UPDATE NO ACTION
        ON DELETE NO ACTION,
    CONSTRAINT "transOrd_trasportatore_fkey" FOREIGN KEY (trasportatore)
        REFERENCES trasportatore (id) MATCH SIMPLE
        ON UPDATE NO ACTION
        ON DELETE NO ACTION
)
CREATE OR REPLACE TRIGGER presa_ordine
    BEFORE INSERT
    ON transord
    FOR EACH STATEMENT
    EXECUTE FUNCTION take_pkg();


CREATE TABLE IF NOT EXISTS trasportatore
(
    id integer NOT NULL DEFAULT nextval('trasportatore_id_seq'::regclass),
    piva character varying(11) COLLATE pg_catalog."default" NOT NULL,
    nome character varying(100) COLLATE pg_catalog."default" NOT NULL,
    indirizzo integer NOT NULL,
    CONSTRAINT trasportatore_pkey PRIMARY KEY (id),
    CONSTRAINT trasportatore_piva_key UNIQUE (piva),
    CONSTRAINT trasportatore_indirizzo_fkey FOREIGN KEY (indirizzo)
        REFERENCES indirizzo (id) MATCH SIMPLE
        ON UPDATE NO ACTION
        ON DELETE NO ACTION,
    CONSTRAINT ivalung CHECK (length(piva::text) = 11)
)



CREATE TABLE IF NOT EXISTS wishlist
(
    id integer NOT NULL DEFAULT nextval('wishlist_id_seq'::regclass),
    customer integer NOT NULL,
    nome character varying(25) COLLATE pg_catalog."default" NOT NULL,
    CONSTRAINT wishlist_pkey PRIMARY KEY (id),
    CONSTRAINT wishlist_customer_nome_key UNIQUE (customer, nome),
    CONSTRAINT wishlist_customer_fkey FOREIGN KEY (customer)
        REFERENCES customers (id) MATCH SIMPLE
        ON UPDATE NO ACTION
        ON DELETE NO ACTION
)
