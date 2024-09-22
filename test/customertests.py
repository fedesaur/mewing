import requests
import random

# Dati di configurazione
HOST = "127.0.0.1"
PORT = 5001
BASE_URL = f"http://{HOST}:{PORT}"

# Lista di email che già esistono nel sistema
email_list = ["abc@abc.it", "cab@cab.it", "bca@bca.it"]

# Variabile per contare i test riusciti
success_count = 0
total_tests = 6  # Numero totale di test da eseguire

# Variabile per memorizzare i prodotti recuperati
prodotti_disponibili = []

# Funzione per ottenere un'email casuale dal sistema
def get_random_email():
    return random.choice(email_list)

### Test per ciascuna rotta ###

# 1. Test autentica
def test_autentica():
    global success_count
    email = get_random_email()
    response = requests.post(f"{BASE_URL}/autentica/{email}")
    if response.status_code == 200:
        print(f"Autenticazione avvenuta con successo per {email}")
        success_count += 1  # Incrementa il contatore in caso di successo
    else:
        print(f"Errore durante l'autenticazione per {email}. Status code: {response.status_code}")

# 2. Test modificaNome
def test_modifica_nome():
    global success_count
    data = {"nome": "NuovoNome"}  # Simula l'invio di un nuovo nome
    response = requests.post(f"{BASE_URL}/modificaNome", json=data)
    if response.status_code == 200:
        print(f"Nome modificato con successo")
        success_count += 1  # Incrementa il contatore in caso di successo
    else:
        print(f"Errore durante la modifica del nome. Status code: {response.status_code}")

# 3. Test recupera prodotti
def test_recupera_prodotti():
    global success_count, prodotti_disponibili
    response = requests.get(f"{BASE_URL}/prodotti")
    if response.status_code == 200:
        print("Prodotti recuperati con successo:")
        #prodotti_disponibili = response.json()  # Memorizza i prodotti recuperati
        #print(prodotti_disponibili)
        #success_count += 1  # Incrementa il contatore in caso di successo
    else:
        print(f"Errore nel recupero dei prodotti. Status code: {response.status_code}")

# 4. Test aggiungi prodotto al carrello
def test_aggiungi_prodotto_al_carrello():
    global success_count

    #if len(prodotti_disponibili) == 0:
        #print("Nessun prodotto disponibile per aggiungere al carrello.")
        #return


    email = get_random_email()
    prodotto_id = random.randint(1, 8)  # Scegli un prodotto casuale
    quantita = random.randint(1, 5)  # Quantità casuale tra 1 e 5

    response = requests.post(f"{BASE_URL}/addToCarrello/{email}/{prodotto_id}/{quantita}")
    if response.status_code == 200:
        print(f"Prodotto aggiunto al carrello per l'utente {email} con quantità {quantita}")
        success_count += 1  # Incrementa il contatore in caso di successo
    else:
        print(f"Errore nell'aggiunta del prodotto al carrello per {email}. Status code: {response.status_code}")

# 5. Test visualizza carrello
def test_visualizza_carrello():
    global success_count
    email = get_random_email()
    response = requests.get(f"{BASE_URL}/carrello/{email}")
    if response.status_code == 200:
        print(f"Carrello recuperato per {email}:")
        print(response.text)
        success_count += 1  # Incrementa il contatore in caso di successo
    else:
        print(f"Errore nel recupero del carrello per {email}. Status code: {response.status_code}")

# 6. Test effettua ordine
def test_ordina():
    global success_count
    email = get_random_email()
    response = requests.post(f"{BASE_URL}/ordina/{email}")
    if response.status_code == 200:
        print(f"Ordine effettuato con successo per {email}")
        success_count += 1  # Incrementa il contatore in caso di successo
    else:
        print(f"Errore durante l'ordine per {email}. Status code: {response.status_code}")

### Funzione principale per eseguire tutti i test ###
def run_all_tests():
    print("Esecuzione dei test automatici...")
    
    print("\n--- Test Autentica ---")
    test_autentica()

    print("\n--- Test Modifica Nome ---")
    test_modifica_nome()

    print("\n--- Test Recupera Prodotti ---")
    test_recupera_prodotti()

    print("\n--- Test Aggiungi Prodotto al Carrello ---")
    test_aggiungi_prodotto_al_carrello()

    print("\n--- Test Visualizza Carrello ---")
    test_visualizza_carrello()

    print("\n--- Test Effettua Ordine ---")
    test_ordina()

    # Stampa dei risultati finali
    print(f"\nTest automatici completati: {success_count} su {total_tests} test sono andati a buon fine.")

if __name__ == "__main__":
    run_all_tests()
