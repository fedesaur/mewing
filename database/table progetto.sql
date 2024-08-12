CREATE TYPE public.dim AS
(
    lunghezza pos,
    larghezza pos,
    altezza pos
);
CREATE TYPE public.statoordine AS ENUM
    ('annullato', 'pendente', 'accettato');

CREATE TYPE public.tipometpag AS ENUM
    ('Virtuale', 'contante', 'buono', 'carta prepagata', 'carta di credito','bancomat');




CREATE TABLE IF NOT EXISTS public.buono
(
    id integer NOT NULL,
    scadenza timestamp without time zone NOT NULL,
    valore pos NOT NULL,
    CONSTRAINT buono_id_fkey FOREIGN KEY (id)
        REFERENCES public.metpag (id) MATCH SIMPLE
        ON UPDATE NO ACTION
        ON DELETE NO ACTION
)



CREATE TABLE IF NOT EXISTS public.carrello
(
    customer integer NOT NULL,
    totale numeric NOT NULL DEFAULT 0,
    CONSTRAINT carrello_customer_key UNIQUE (customer),
    CONSTRAINT carrello_customer_fkey FOREIGN KEY (customer)
        REFERENCES public.customers (id) MATCH SIMPLE
        ON UPDATE NO ACTION
        ON DELETE NO ACTION
)



CREATE TABLE IF NOT EXISTS public.consegna
(
    ordine integer NOT NULL,
    corriere integer NOT NULL,
    CONSTRAINT consegna_pkey PRIMARY KEY (ordine, corriere),
    CONSTRAINT consegna_corriere_fkey FOREIGN KEY (corriere)
        REFERENCES public.corriere (id) MATCH SIMPLE
        ON UPDATE NO ACTION
        ON DELETE NO ACTION,
    CONSTRAINT consegna_ordine_fkey FOREIGN KEY (ordine)
        REFERENCES public.ordine (id) MATCH SIMPLE
        ON UPDATE NO ACTION
        ON DELETE NO ACTION
)



-- Trigger: same_trans

-- DROP TRIGGER IF EXISTS same_trans ON public.consegna;

CREATE OR REPLACE TRIGGER same_trans
    BEFORE INSERT
    ON public.consegna
    FOR EACH ROW
    EXECUTE FUNCTION public.check_del();


CREATE TABLE IF NOT EXISTS public.corriere
(
    id integer NOT NULL DEFAULT nextval('corriere_id_seq'::regclass),
    azienda integer NOT NULL,
    nome character varying(20) COLLATE pg_catalog."default" NOT NULL,
    cognome character varying(20) COLLATE pg_catalog."default" NOT NULL,
    CONSTRAINT corriere_pkey PRIMARY KEY (id),
    CONSTRAINT corriere_azienda_fkey FOREIGN KEY (azienda)
        REFERENCES public.trasportatore (id) MATCH SIMPLE
        ON UPDATE NO ACTION
        ON DELETE NO ACTION
)



CREATE TABLE IF NOT EXISTS public.custadd
(
    customer integer NOT NULL,
    addr integer NOT NULL,
    CONSTRAINT custadd_pkey PRIMARY KEY (customer, addr),
    CONSTRAINT custadd_addr_fkey FOREIGN KEY (addr)
        REFERENCES public.indirizzo (id) MATCH SIMPLE
        ON UPDATE NO ACTION
        ON DELETE NO ACTION,
    CONSTRAINT custadd_customer_fkey FOREIGN KEY (customer)
        REFERENCES public.customers (id) MATCH SIMPLE
        ON UPDATE NO ACTION
        ON DELETE NO ACTION
)



CREATE TABLE IF NOT EXISTS public.customers
(
    id integer NOT NULL DEFAULT nextval('customer_id_seq'::regclass),
    nome character varying(20) COLLATE pg_catalog."default",
    cognome character varying(20) COLLATE pg_catalog."default",
    mail character varying(50) COLLATE pg_catalog."default",
    abita integer,
    CONSTRAINT customers_pkey PRIMARY KEY (id),
    CONSTRAINT customers_abita_fkey FOREIGN KEY (abita)
        REFERENCES public.indirizzo (id) MATCH SIMPLE
        ON UPDATE NO ACTION
        ON DELETE NO ACTION
)

CREATE TABLE IF NOT EXISTS public.fornitore
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
        REFERENCES public.indirizzo (id) MATCH SIMPLE
        ON UPDATE NO ACTION
        ON DELETE NO ACTION,
    CONSTRAINT lengpiva CHECK (length(piva::text) = 11)
)

CREATE TABLE IF NOT EXISTS public.indirizzo
(
    id integer NOT NULL DEFAULT nextval('addr_id_sequence'::regclass),
    via character varying(100) COLLATE pg_catalog."default" NOT NULL,
    civico pos,
    cap character varying(5) COLLATE pg_catalog."default" NOT NULL,
    citta character varying(30) COLLATE pg_catalog."default" NOT NULL,
    stato character varying(30) COLLATE pg_catalog."default" NOT NULL,
    CONSTRAINT indirizzo_pkey PRIMARY KEY (id)
)

CREATE TABLE IF NOT EXISTS public.inwish
(
    list integer NOT NULL,
    prodotto integer NOT NULL,
    CONSTRAINT inwish_pkey PRIMARY KEY (list, prodotto),
    CONSTRAINT inwish_list_fkey FOREIGN KEY (list)
        REFERENCES public.wishlist (id) MATCH SIMPLE
        ON UPDATE NO ACTION
        ON DELETE NO ACTION,
    CONSTRAINT inwish_prodotto_fkey FOREIGN KEY (prodotto)
        REFERENCES public.prodotto (id) MATCH SIMPLE
        ON UPDATE NO ACTION
        ON DELETE NO ACTION
)

CREATE TABLE IF NOT EXISTS public.metpag
(
    id integer NOT NULL DEFAULT nextval('metpag_id_seq'::regclass),
    nome character varying(50) COLLATE pg_catalog."default" NOT NULL,
    tipo tipometpag NOT NULL,
    Customer integer,
	foreign key (customer) reference customers(id),

    CONSTRAINT metpag_pkey PRIMARY KEY (id),
    
)


CREATE TABLE IF NOT EXISTS public.ordine
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
        REFERENCES public.indirizzo (id) MATCH SIMPLE
        ON UPDATE NO ACTION
        ON DELETE NO ACTION
        NOT VALID,
    CONSTRAINT "metPag" FOREIGN KEY (pagamento)
        REFERENCES public.metpag (id) MATCH SIMPLE
        ON UPDATE NO ACTION
        ON DELETE NO ACTION
        NOT VALID,
    CONSTRAINT ordine_customer_fkey FOREIGN KEY (customer)
        REFERENCES public.customers (id) MATCH SIMPLE
        ON UPDATE NO ACTION
        ON DELETE NO ACTION
)
CREATE OR REPLACE TRIGGER same_addr
    BEFORE INSERT
    ON public.ordine
    FOR EACH ROW
    EXECUTE FUNCTION public.check_addr();



CREATE TABLE IF NOT EXISTS public.ordineconse
(
    id integer NOT NULL,
    datacons timestamp without time zone NOT NULL,
    CONSTRAINT ordineconse_pkey PRIMARY KEY (id),
    CONSTRAINT ordineconse_id_fkey FOREIGN KEY (id)
        REFERENCES public.ordine (id) MATCH SIMPLE
        ON UPDATE NO ACTION
        ON DELETE NO ACTION
)


CREATE TABLE IF NOT EXISTS public.prodincart
(
    carrello integer NOT NULL,
    prodotto integer NOT NULL,
    quantita integer,
    CONSTRAINT prodincart_pkey PRIMARY KEY (carrello, prodotto),
    CONSTRAINT prodincart_carrello_fkey FOREIGN KEY (carrello)
        REFERENCES public.carrello (customer) MATCH SIMPLE
        ON UPDATE NO ACTION
        ON DELETE NO ACTION,
    CONSTRAINT prodincart_prodotto_fkey FOREIGN KEY (prodotto)
        REFERENCES public.prodotto (id) MATCH SIMPLE
        ON UPDATE NO ACTION
        ON DELETE NO ACTION
)
CREATE OR REPLACE TRIGGER buy_price
    BEFORE INSERT
    ON public.prodincart
    FOR EACH ROW
    EXECUTE FUNCTION public.tot_cart();



CREATE TABLE IF NOT EXISTS public.prodinord
(
    prodotto integer NOT NULL,
    ordine integer NOT NULL,
    quantita integer NOT NULL DEFAULT 1,
    CONSTRAINT prodinord_pkey PRIMARY KEY (ordine, prodotto),
    CONSTRAINT prodinord_ordine_fkey FOREIGN KEY (ordine)
        REFERENCES public.ordine (id) MATCH SIMPLE
        ON UPDATE NO ACTION
        ON DELETE NO ACTION,
    CONSTRAINT prodinord_prodotto_fkey FOREIGN KEY (prodotto)
        REFERENCES public.prodotto (id) MATCH SIMPLE
        ON UPDATE NO ACTION
        ON DELETE NO ACTION
)
CREATE OR REPLACE TRIGGER same_meth
    BEFORE INSERT
    ON public.prodinord
    FOR EACH ROW
    EXECUTE FUNCTION public.check_meth();



CREATE TABLE IF NOT EXISTS public.prodmet
(
    prodotto integer NOT NULL,
    metodo tipometpag NOT NULL,
    CONSTRAINT prodmet_pkey PRIMARY KEY (prodotto, metodo),
    
    CONSTRAINT prodmet_prodotto_fkey FOREIGN KEY (prodotto)
        REFERENCES public.prodotto (id) MATCH SIMPLE
        ON UPDATE NO ACTION
        ON DELETE NO ACTION
)

CREATE TABLE IF NOT EXISTS public.prodotto
(
    id integer NOT NULL DEFAULT nextval('prodotto_id_seq'::regclass),
    descrizione text COLLATE pg_catalog."default" NOT NULL,
    prezzo double precision NOT NULL,
    dimensione dim NOT NULL,
    fornitore integer NOT NULL,
    nome character varying(100) COLLATE pg_catalog."default" NOT NULL,
    CONSTRAINT prodotto_pkey PRIMARY KEY (id),
    CONSTRAINT prodotto_fornitore_fkey FOREIGN KEY (fornitore)
        REFERENCES public.fornitore (id) MATCH SIMPLE
        ON UPDATE NO ACTION
        ON DELETE NO ACTION
)

CREATE TABLE IF NOT EXISTS public.transord
(
    ordine integer NOT NULL,
    trasportatore integer NOT NULL,
    CONSTRAINT "transOrd_pkey" PRIMARY KEY (ordine, trasportatore),
    CONSTRAINT "transOrd_ordine_fkey" FOREIGN KEY (ordine)
        REFERENCES public.ordine (id) MATCH SIMPLE
        ON UPDATE NO ACTION
        ON DELETE NO ACTION,
    CONSTRAINT "transOrd_trasportatore_fkey" FOREIGN KEY (trasportatore)
        REFERENCES public.trasportatore (id) MATCH SIMPLE
        ON UPDATE NO ACTION
        ON DELETE NO ACTION
)
CREATE OR REPLACE TRIGGER presa_ordine
    BEFORE INSERT
    ON public.transord
    FOR EACH STATEMENT
    EXECUTE FUNCTION public.take_pkg();


CREATE TABLE IF NOT EXISTS public.trasportatore
(
    id integer NOT NULL DEFAULT nextval('trasportatore_id_seq'::regclass),
    piva character varying(11) COLLATE pg_catalog."default" NOT NULL,
    nome character varying(100) COLLATE pg_catalog."default" NOT NULL,
    indirizzo integer NOT NULL,
    CONSTRAINT trasportatore_pkey PRIMARY KEY (id),
    CONSTRAINT trasportatore_piva_key UNIQUE (piva),
    CONSTRAINT trasportatore_indirizzo_fkey FOREIGN KEY (indirizzo)
        REFERENCES public.indirizzo (id) MATCH SIMPLE
        ON UPDATE NO ACTION
        ON DELETE NO ACTION,
    CONSTRAINT ivalung CHECK (length(piva::text) = 11)
)



CREATE TABLE IF NOT EXISTS public.wishlist
(
    id integer NOT NULL DEFAULT nextval('wishlist_id_seq'::regclass),
    customer integer NOT NULL,
    nome character varying(25) COLLATE pg_catalog."default" NOT NULL,
    CONSTRAINT wishlist_pkey PRIMARY KEY (id),
    CONSTRAINT wishlist_customer_nome_key UNIQUE (customer, nome),
    CONSTRAINT wishlist_customer_fkey FOREIGN KEY (customer)
        REFERENCES public.customers (id) MATCH SIMPLE
        ON UPDATE NO ACTION
        ON DELETE NO ACTION
)