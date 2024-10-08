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
create or replace function into_ord() returns trigger as $cart_To_ord$
	BEGIN
		insert into prodinord(prodotto, ordine, quantita)
			select pc.carrello, NEW.id, pc.quantita
			from prodincart pc
			where NEW.customer=pc.carrello;
		update ordine set totale=(select c.totale
						from carrello c
						where c.customer=NEW.customer);
		delete from prodincart where carrello=NEW.customer;
		update carrello set totale=0 where customer=NEW.customer;
	    RETURN NEW;
    END
	$cart_To_ord$
	language plpgsql;


create or replace TRIGGER cart_To_ord after insert on ordine for each row execute procedure into_ord();
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
            from  carrello c, prodotto p
            where c.customer=NEW.carrello 
            and NEW.prodotto=p.id)
     where carrello.customer=NEW.carrello;
        
        
        return NEW;
    END
    $buy_price$
    language plpgsql;


create or replace TRIGGER buy_price after insert or update on prodincart
for each row execute procedure tot_cart();

------------------------------------------------------------------------
create or replace function no_double_cart() RETURNS TRIGGER AS $check_cart$
    BEGIN
    	if exists(select * 
			from prodincart pc 
			where pc.carrello=NEW.carrello and pc.prodotto=NEW.prodotto
			) then update prodincart set quantita=(select (pc.quantita + NEW.quantita)
								from prodincart pc
								where pc.prodotto=NEW.prodotto
								and pc.carrello=NEW.carrello)
				where prodincart.prodotto=NEW.prodotto and prodincart.carrello=NEW.carrello;
				RETURN NULL;
       												
		else RETURN NEW;
		END IF;
    END
   $check_cart$
   language plpgsql;
   create or replace TRIGGER check_cart before insert on prodincart
for each row execute procedure no_double_cart();
------------------------------------------------------------------------
create or replace function tot_cart_2() RETURNS TRIGGER AS $buy_price2$
    BEGIN
    	if not exists(select * 
			from carrello c, prodotto p, prodincart pc
			where OLD.carrello=pc.carrello
			) then update carrello set totale=0 where carrello.customer=OLD.carrello;
		else update carrello
    		set totale=(select SUM((p.prezzo * OLD.quantita)) 
            		from  carrello c, prodotto p
            		where c.customer=OLD.carrello 
            		and OLD.prodotto=p.id)
     				where carrello.customer=OLD.carrello;
		END IF;

     	RETURN OLD;
    END
   $buy_price2$
   language plpgsql;
   create or replace TRIGGER buy_price2 after delete on prodincart
for each row execute procedure tot_cart_2();

------------------------------------------------------------------------

create or replace TRIGGER buy_price2 after delete on prodincart
for each row execute procedure tot_cart_2();

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
                    from transord ot, ordine o, corriere c
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
				where pm.prodotto=NEW.prodotto
				and o.id=NEW.ordine and o.pagamento=pm.metodo
                    ) THEN return NEW;

        ELSE  return null;

        end if;
    END
    $same_meth$
    LANGUAGE plpgsql;


create or replace TRIGGER same_meth before insert on prodinord
for each row execute Procedure check_meth();
