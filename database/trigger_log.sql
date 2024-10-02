\c :log_dbname

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
CREATE OR REPLACE FUNCTION consistent_Operation() RETURNS TRIGGER AS $$
BEGIN
	IF EXISTS(
			SELECT 1
				FROM cliente
				WHERE NEW.User_Id=client.User_Id
				AND (cliente.Data_termine IS NULL OR NEW.Data_inizio <= cliente.Data_termine)
		)	IS TRUE THEN
				RAISE EXCEPTION 'connessione file descriptor non chiusa';
	END IF;

	RETURN NEW;
END;

$$ LANGUAGE plpgsql;

CREATE TRIGGER consistent_Operation_trg
	BEFORE INSERT ON cliente FOR EACH ROW EXECUTE FUNCTION consistent_Operation();
