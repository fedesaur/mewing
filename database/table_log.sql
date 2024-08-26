CREATE DOMAIN Pos as integer check (value>0);
CREATE TYPE TipoStatisticheSessione AS ENUM ('Sessione', 'Risposta');

CREATE TABLE cliente(
					nome_server VARCHAR(50) NOT NULL,
					file_descriptor integer NOT NULL,
					tempo_connessione timestamp NOT NULL,
					tempo_disconnessione timestamp,
					PRIMARY KEY(nome_server, file_descriptor,tempo_connessione),
					CONSTRAINT check_tempo_disconnessione CHECK(tempo_disconnessione IS NULL OR tempo_disconnessione>= tempo_connessione)
					);

CREATE TABLE comunicazione(
			richiesta VARCHAR(1000) NOT NULL,
			tempo_richiesta timestamp NOT NULL,
			stato_risposta VARCHAR(50),
			tempo_risposta timestamp ,
			nome_server_cliente VARCHAR(50) NOT NULL,
			descrizione_file_cliente integer NOT NULL,
			tempo_connessione_cliente timestamp NOT NULL,

			PRIMARY KEY(nome_server_cliente, descrizione_file_cliente, tempo_connessione_cliente, tempo_richiesta),
			FOREIGN KEY(nome_server_cliente, descrizione_file_cliente, tempo_connessione_cliente) REFERENCES cliente (nome_server, file_descriptor, tempo_connessione),
			CONSTRAINT risposta_dopo_richiesta CHECK (tempo_risposta IS NULL OR tempo_risposta>=tempo_richiesta),
			CONSTRAINT richiesta_dopo_connessione CHECK(tempo_richiesta>=tempo_connessione_cliente)
			);

CREATE TABLE statisticheSessione(
			id SERIAL NOT NULL PRIMARY KEY,
			tipo TipoStatisticheSessione NOT NULL,
			istante_finale timestamp NOT NULL,
			valore Pos NOT NULL,
			stato_risposta VARCHAR(50) NOT NULL,

			UNIQUE (tipo, istante_finale)
			);
