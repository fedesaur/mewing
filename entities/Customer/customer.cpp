#include "customer.h"

// Costruttore di Customer
Customer::Customer(
    int id,
    std::string nome,
    std::string cognome,
    std::string mail,
    int città
)
{
    // Effettuati controlli sui parametri per fare in modo che rispettino i limiti richiesti
    assert(nome.length() > 0 && nome.length() <= 20);
    assert(cognome.length() > 0 && cognome.length() <= 20);
    assert(mail.length() > 0 && mail.length() <= 50);

    ID = id;
    Nome = nome;
    Cognome = cognome;
    Mail = mail;
    Abita = città;


    /* Effettua la connessione a Redis
    (Anche qui, controllare se conviene metterlo in un main)
    */
    c2r = redisConnect(REDIS_IP, REDIS_PORT);

    // In caso già esistano, elimina i due stream di lettura e scrittura
    reply = RedisCommand(c2r, "DEL %s", READ_STREAM);
    assertReply(c2r, reply);
    dumpReply(reply, 0);

    reply = RedisCommand(c2r, "DEL %s", WRITE_STREAM);
    assertReply(c2r, reply);
    dumpReply(reply, 0);

    // Crea gli stream per lettura e scrittura
    initStreams(c2r, READ_STREAM);
	initStreams(c2r, WRITE_STREAM);


    /* Effettua la connessione al server:
    Server(char* RedisIP, int RedisPort, int serverPort, char* streamIN, char* streamOUT);
    */
	Server Server(REDIS_IP, REDIS_PORT, 160, WRITE_STREAM, READ_STREAM);

}
void AggiungiIndirizzo(
    std::string via,
    int civico,
    std::string cap,
    std::string city,
    std::string stato
)
{
    // Effettuati controlli sui parametri per fare in modo che rispettino i limiti richiesti
    assert(via.length() > 0 && via.length() <= 100);
    assert(civico >= 0);
    assert(cap.length() == 5);
    assert(city.length() > 0 && city.length() <= 30);
    assert(stato.length() > 0 && stato.length() <= 30);
}

int main()
{
    AggiungiIndirizzo("Viottolo", 3, "01234", "Roma", "Italia");
    return 0;
}