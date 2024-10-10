## mewing
Progetto di Ingegneria del Software: Backend di un sito di e-commerce

Per poter eseguire il progetto, è necessario prima installare le librerie necessarie per la compilazione eseguendo il seguente comando da terminale:
```
./install.sh
```

Installate le librerie, bisogna spostarsi nella cartella **"database"** ed eseguire il seguente comando da terminale:
```
./create.sh
```
In questo modo, verrà creato il database per l'esecuzione delle query da parte del server.
Scaricate le librerie e creato il database, bisogna spostarsi nella cartella **"servers"**, scegliere uno dei tre server resi disponibili, Customer, Fornitore o Trasportatore, e spostarsi nella cartella **"src"** del server scelto. Nella cartella src, è necessario eseguire il comando:
```
make
```
Tale comando attiverà la compilazione dei file nel server presenti nella cartella e anche quelli presenti nella cartella **"metodi"**, generando un unico eseguibile che verrà locato in **"../bin"**. Creato l'eseguibile, bisogna spostarsi in **"bin"** ed eseguire il comando:
```
./main
```
Così facendo, il server verrà avviato. Ora sarà necessario aprire una nuova pagina nel terminale per poter comunicare con il server. I comandi eseguibili sono visionabili nel file **"routes.cpp"** nella cartella **"src"** del server scelto. Un esempio è:
```
curl -X GET http://localhost:5001/autentica/abc@abc.it
> Permette di effettuare l'autenticazione nel Customer
```

Studenti che hanno collaborato al progetto:
-Edoardo Bouquillon (2014320);
-Simone Camagna (1997716);
-Federica Sardo (1760539);
