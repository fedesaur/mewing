CREATE OR REPLACE FUNCTION check_addr()
    RETURNS trigger
    LANGUAGE 'plpgsql'
    COST 100
    VOLATILE NOT LEAKPROOF
AS $BODY$
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
    
$BODY$;

CREATE OR REPLACE FUNCTION check_del()
    RETURNS trigger
    LANGUAGE 'plpgsql'
    COST 100
    VOLATILE NOT LEAKPROOF
AS $BODY$
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
        
$BODY$;


CREATE OR REPLACE FUNCTION check_meth()
    RETURNS trigger
    LANGUAGE 'plpgsql'
    COST 100
    VOLATILE NOT LEAKPROOF
AS $BODY$
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
    
$BODY$;



CREATE OR REPLACE FUNCTION tot_cart()
    RETURNS trigger
    LANGUAGE 'plpgsql'
    COST 100
    VOLATILE NOT LEAKPROOF
AS $BODY$
    BEGIN
    update carrello
    set totale=(select SUM((p.prezzo * NEW.quantita)) 
            from  carrello c, prodotto p
            where c.customer=NEW.carrello 
            and NEW.prodotto=p.id)
     where carrello.customer=NEW.carrello;
        
        
        return NEW;
    END
    
$BODY$;


