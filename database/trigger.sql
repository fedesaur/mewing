\c :dbname 

-- drop all functions and triggers
DO $$
DECLARE
    function_name TEXT;
BEGIN
    FOR function_name IN
        SELECT p.proname
        FROM pg_proc p
        INNER JOIN pg_namespace n ON p.pronamespace = n.oid
        WHERE n.nspname = 'public'
    LOOP
        EXECUTE 'DROP FUNCTION IF EXISTS ' || function_name || ' CASCADE';
    END LOOP;
END $$;

-------------------------------------------------------------------------
create or replace function take_pkg() returns trigger as $presa_ordine$
    BEGIN
        update ordine
        set stato='accettato'
        where ordine.id=NEW.ordine;
        return NEW;
    END
    $presa_ordine$
        language plpgsql;

create or replace TRIGGER presa_ordine before insert on transord
execute PROCEDURE take_pkg();


----------------------------------------------------------------------
create or replace function tot_cart() returns trigger as $buy_price$
    BEGIN
    update carrello
    set totale=(select SUM((p.prezzo * NEW.quantita)) 
            from  carello c, prodotto p
            where c.customer=NEW.carrello 
            and NEW.prodotto=p.id)
     where carrello.customer=NEW.carrello;
        
        
        return NEW;
    END
    $buy_price$
    language plpgsql;


create or replace TRIGGER buy_price before insert on prodincart
for each row execute procedure tot_cart();

------------------------------------------------------------------------
create or replace function check_addr() returns trigger as $same_addr$
    BEGIN  
        if EXISTS(
            select * 
                from custadd ca 
                where NEW.customer=ca.customer
                and NEW.indirizzo=ca.addr

        )then return NEW;
        ELSE return null;
        end if;
    END
    $same_addr$
    LANGUAGE plpgsql;

create or replace TRIGGER same_addr before insert on ordine
for each row execute procedure check_addr();


------------------------------------------------------------------------
create or replace function check_del() returns trigger as $same_trans$
    BEGIN
        if exists(
                select *
                    from ordTrans ot, ordine o, corriere c
                    where NEW.corriere=c.id
                    and o.id=NEW.ordine
                    and o.id=ot.ordine
                    and c.azienda=ot.trasportatore
        ) then return NEW;
        else return null;
        end if;
    END
        $same_trans$
        LANGUAGE plpgsql;
create or replace TRIGGER same_trans before insert on consegna
for each row execute procedure check_del();



------------------------------------------------------------------------
CREATE or REPLACE FUNCTION check_meth() RETURNS trigger as $same_meth$
    BEGIN
         if EXISTS(
                        select * 
                            From ordine o, prodmet pm
				where pm.prod=NEW.prodotto
				and o.id=NEW.ordine and o.pagamento=pm.metodo
                    ) THEN return NEW;

        ELSE  return null;

        end if;
    END
    $same_meth$
    LANGUAGE plpgsql;


create or replace TRIGGER same_meth before insert on prodinord
for each row execute Procedure check_meth();
