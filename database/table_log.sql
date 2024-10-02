\c :log_dbname
CREATE type TipoUtenteType as ENUM ('customer','produttore','trasportatore');
CREATE type EsitoType as ENUM ('Successo','Fallito');
CREATE sequence Cliente_id_sequence;

CREATE TABLE cliente(Id integer not null DEFAULT nextval('Cliente_id_sequence'::regclass),
		     Cliente_id Integer not null
		     TipoUser TipoUtenteType not null,
		     OperationTipe varchar(50) not null,
		     Data_inizio timestamp not null,
	   	     Data_termine timestamp,
	 	     Esito EsitoType,
		     
			PRIMARY KEY (Id),
			contraint unique Log UNIQUE(User_Id,TipoUser,Data_inizio),
			CONSTRAINT check_tempo_inizio_fine check(Data_termine IS NULL OR Data_termine>=Data_inizio)
			);


