\c :log_dbname
CREATE type TipoUtenteType as ENUM ('customer','produttore','trasportatore');
CREATE type EsitoType as ENUM ('Successo','Fallito');

CREATE TABLE cliente(User_Id integer not null,
		     TipoUser TipoUtenteType not null,
		     OperationTipe varchar(50) not null,
		     Data_inizio timestamp not null,
	   	     Data_termine timestamp,
	 	     Esito EsitoType,
		     
			PRIMARY KEY (User_Id,TipoUser,Data_inizio),
			CONSTRAINT check_tempo_inizio_fine check(Data_termine IS NULL OR Data_termine>=Data_inizio)
			);


