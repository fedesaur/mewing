\c :dbname
insert into indirizzo (via, civico, cap, citta, stato) values('ortana',940,05035,'narni', 'italia');
insert into indirizzo (via, civico, cap, citta, stato) values('del corso',1,00157,'roma', 'italia');
insert into indirizzo (via, civico, cap, citta, stato) values('roma',53,00100,'viterbo', 'italia');
insert into indirizzo (via, civico, cap, citta, stato) values('salaria',3,00150,'roma', 'italia');
insert into indirizzo (via, civico, cap, citta, stato) values('abcde',30,12345,'vetralla', 'italia');
insert into indirizzo (via, civico, cap, citta, stato) values('prima',26,20092,'cinisello balsamo', 'italia');
insert into indirizzo (via, civico, cap, citta, stato) values('lungomare',99,70044,'polignano a mare', 'italia');
insert into indirizzo (via, civico, cap, citta, stato) values('montanara',404,44,'monognano a montagna', 'italia');


insert into customers (nome, cognome, mail, abita) values ('edoardo','bughi','abc@abc.it',1);
insert into customers (nome, cognome, mail, abita) values ('simone','camagna','bca@bca.it',2);
insert into customers (nome, cognome, mail, abita) values ('federica','sardo','cab@cab.it',3);


insert into fornitore (nome, piva, mail,telefono, sede) values('noscam','12345678901','prova1@prova1.it','+393391234567',4);
insert into fornitore (nome, piva, mail,telefono, sede) values('scam','09876543210','prova2@prova2.it','+393930987654',5);


insert into prodotto (descrizione, prezzo, fornitore, nome) values('un computer', 999.99, 1,'hp omen salcazzo');
insert into prodotto (descrizione, prezzo, fornitore, nome) values('un condizionatore', 999.99, 1, 'refresh me plus better');
insert into prodotto (descrizione, prezzo, fornitore, nome) values('un computer verissimo', 555.99, 2,'lp hommen');
insert into prodotto (descrizione, prezzo, fornitore, nome) values('un telefono', 600.99, 1,'iphone 27 pro max ultra+');
insert into prodotto (descrizione, prezzo, fornitore, nome) values('un orologio', 250.99, 2,'sassong galassia guarda 4');
insert into prodotto (descrizione, prezzo, fornitore, nome) values('ukulele', 299.99, 1,'lava Mi');
insert into prodotto (descrizione, prezzo, fornitore, nome) values('tablet', 599.99, 2,'best tablet evah');


insert into trasportatore (piva, nome, indirizzo) values ('32132132132','gls',6);
insert into trasportatore (piva, nome, indirizzo) values ('45645645645','ups',7);
insert into trasportatore (piva, nome, indirizzo) values ('78978978978','ammiocuggino',8);


insert into metpag(nome, tipo, customer) values ('intesa San Pietro','bancomat', 1);
insert into metpag(nome, tipo, customer) values ('Paga Posta','carta prepagata', 2);
insert into metpag(nome, tipo, customer) values ('colle dei boschi di pisa','carta di credito', 3);


insert into prodmet(prod, metodo) values (1,'bancomat');
insert into prodmet(prod, metodo) values (1,'carta di credito');
insert into prodmet(prod, metodo) values (2, 'bancomat');
insert into prodmet(prod, metodo) values (3, 'carta di credito');
insert into prodmet(prod, metodo) values (4,'bancomat');
insert into prodmet(prod, metodo) values (4, 'carta prepagata');
insert into prodmet(prod, metodo) values (5, 'carta prepagata');
insert into prodmet(prod, metodo) values (5, 'carta di credito');
insert into prodmet(prod, metodo) values (6, 'carta di credito');
insert into prodmet(prod, metodo) values (6, 'bancomat');
insert into prodmet(prod, metodo) values (7, 'bancomat');
insert into prodmet(prod, metodo) values (7, 'carta di credito');



