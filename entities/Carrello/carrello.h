#define carrello_h


/* Classe Carrello con i seguenti attributi
*/
class Carrello{
    public: 
        int Customerid = 0;
        int Totale = 0;
        std::list int carrello; 

        Carrello(int customerid, int totale);
};