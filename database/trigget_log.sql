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


CREATE OR REPLACE FUNCTION consistent_file_descriptor() RETURNS TRIGGER AS $$
BEGIN
	IF EXISTS(
			SELECT 1
				FROM cliente
				WHERE NEW.nome_server=client.nome_server
				AND NEW.file_descriptor=cliente.file_descriptor
				AND (cliente.tempo_disconnessione IS NULL OR NEW.tempo_connessione <= cliente.tempo_disconnessione)
		)	IS TRUE THEN
				RAISE EXCEPTION 'connessione file descriptor non chiusa';
	END IF;

	RETURN NEW;
END;

$$ LANGUAGE plpgsql;

CREATE TRIGGER consistent_file_descriptor_trg
	BEFORE INSERT ON cliente FOR EACH ROW EXECUTE FUNCTION consistent_file_descriptor();