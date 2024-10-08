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
	update cliente set Data_termine=now() where Cliente_id = NEW.id and Data_termine is null;

	RETURN NEW;
END;

$$ LANGUAGE plpgsql;

CREATE TRIGGER consistent_Operation_trg
	BEFORE INSERT ON cliente FOR EACH ROW EXECUTE FUNCTION consistent_Operation();
