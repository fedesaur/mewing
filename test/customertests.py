import requests
import random
import concurrent.futures
import threading

# Dati di configurazione
HOST = "127.0.0.1"
PORT = 5001
BASE_URL = f"http://{HOST}:{PORT}"

# Lista di email che già esistono nel sistema
email_list = ["abc@abc.it"]

# Lock per gestire l'accesso concorrente al contatore
lock = threading.Lock()

# Variabile per contare i test riusciti
success_count = 0
total_tests = 11 * len(email_list)  # Aggiornato per i nuovi test

# Timeout per le richieste
REQUEST_TIMEOUT = 10  # Timeout di 10 secondi

# Funzione per aggiornare il contatore dei successi in modo thread-safe
def increment_success_count():
    global success_count
    with lock:
        success_count += 1

### Test per ciascuna rotta ###

# 1. Test autentica
def test_autentica(email):
    try:
        response = requests.get(f"{BASE_URL}/autentica/{email}", timeout=REQUEST_TIMEOUT)
        if response.status_code == 200:
            print(f"Autenticazione avvenuta con successo per {email}")
            increment_success_count()
        else:
            print(f"Errore durante l'autenticazione per {email}. Status code: {response.status_code}")
    except requests.exceptions.RequestException as e:
        print(f"Errore nella richiesta per l'autenticazione: {e}")

# 2. Test crea nuovo customer
def test_crea_customer(email):
    try:
        data = {"email": email, "password": "password123"}
        response = requests.put(f"{BASE_URL}/autentica/", json=data, timeout=REQUEST_TIMEOUT)
        if response.status_code == 200:
            print(f"Customer creato con successo per {email}")
            increment_success_count()
        else:
            print(f"Errore durante la creazione del customer per {email}. Status code: {response.status_code}")
    except requests.exceptions.RequestException as e:
        print(f"Errore nella richiesta per la creazione del customer: {e}")

# 3. Test modifica informazioni customer
def test_modifica_info(email):
    try:
        data = {"nome": "Nuovo Nome", "cognome": "Nuovo Cognome"}
        response = requests.post(f"{BASE_URL}/{email}/", json=data, timeout=REQUEST_TIMEOUT)
        if response.status_code == 200:
            print(f"Informazioni del customer modificate con successo per {email}")
            increment_success_count()
        else:
            print(f"Errore nella modifica delle informazioni del customer per {email}. Status code: {response.status_code}")
    except requests.exceptions.RequestException as e:
        print(f"Errore nella richiesta per la modifica delle informazioni del customer: {e}")

# 4. Test recupera indirizzi
def test_recupera_indirizzi(email):
    try:
        response = requests.get(f"{BASE_URL}/{email}/indirizzi/", timeout=REQUEST_TIMEOUT)
        if response.status_code == 200:
            print(f"Indirizzi recuperati con successo per {email}")
            increment_success_count()
        else:
            print(f"Errore durante il recupero degli indirizzi per {email}. Status code: {response.status_code}")
    except requests.exceptions.RequestException as e:
        print(f"Errore nella richiesta per il recupero degli indirizzi: {e}")

# 5. Test rimuovi indirizzo
def test_remove_indirizzo(email, addressID=1):
    try:
        response = requests.delete(f"{BASE_URL}/{email}/indirizzi/{addressID}", timeout=REQUEST_TIMEOUT)
        if response.status_code == 200:
            print(f"Indirizzo rimosso con successo per {email}")
            increment_success_count()
        else:
            print(f"Errore nella rimozione dell'indirizzo per {email}. Status code: {response.status_code}")
    except requests.exceptions.RequestException as e:
        print(f"Errore nella richiesta per la rimozione dell'indirizzo: {e}")

# 6. Test recupera ordini
def test_recupera_ordini(email):
    try:
        response = requests.get(f"{BASE_URL}/{email}/ordini/", timeout=REQUEST_TIMEOUT)
        if response.status_code == 200:
            print(f"Ordini recuperati con successo per {email}")
            increment_success_count()
        else:
            print(f"Errore durante il recupero degli ordini per {email}. Status code: {response.status_code}")
    except requests.exceptions.RequestException as e:
        print(f"Errore nella richiesta per il recupero degli ordini: {e}")

# 7. Test annulla ordine
def test_annulla_ordine(email, ordineID=1):
    try:
        response = requests.delete(f"{BASE_URL}/{email}/ordini/{ordineID}", timeout=REQUEST_TIMEOUT)
        if response.status_code == 200:
            print(f"Ordine annullato con successo per {email}")
            increment_success_count()
        else:
            print(f"Errore durante l'annullamento dell'ordine per {email}. Status code: {response.status_code}")
    except requests.exceptions.RequestException as e:
        print(f"Errore nella richiesta per l'annullamento dell'ordine: {e}")

# 8. Test recupera prodotti
def test_recupera_prodotti(email):
    try:
        response = requests.get(f"{BASE_URL}/{email}/prodotti/", timeout=REQUEST_TIMEOUT)
        if response.status_code == 200:
            print("Prodotti recuperati con successo")
            increment_success_count()
        else:
            print(f"Errore nel recupero dei prodotti. Status code: {response.status_code}")
    except requests.exceptions.RequestException as e:
        print(f"Errore nella richiesta per il recupero dei prodotti: {e}")

# 9. Test aggiungi prodotto al carrello
def test_aggiungi_prodotto_al_carrello(email, quantita=1, IDprodotto=3):
    try:
        response = requests.put(f"{BASE_URL}/{email}/carrello/", json={"quantita": quantita, "IDprodotto": IDprodotto}, timeout=REQUEST_TIMEOUT)
        if response.status_code == 200:
            print(f"Prodotto aggiunto al carrello per {email}")
            increment_success_count()
        else:
            print(f"Errore nell'aggiunta del prodotto al carrello per {email}. Status code: {response.status_code}")
    except requests.exceptions.RequestException as e:
        print(f"Errore nella richiesta per l'aggiunta del prodotto al carrello: {e}")

# 10. Test rimuovi prodotto dal carrello
def test_remove_prodotto_carrello(email, productID=3):
    try:
        response = requests.delete(f"{BASE_URL}/{email}/carrello/{productID}", timeout=REQUEST_TIMEOUT)
        if response.status_code == 200:
            print(f"Prodotto rimosso dal carrello per {email}")
            increment_success_count()
        else:
            print(f"Errore nella rimozione del prodotto dal carrello per {email}. Status code: {response.status_code}")
    except requests.exceptions.RequestException as e:
        print(f"Errore nella richiesta per la rimozione del prodotto dal carrello: {e}")

# 11. Test effettua ordine
def test_ordina(email):
    try:
        response = requests.put(f"{BASE_URL}/{email}/ordini/", json={"pagamento": 'bancomat', "indirizzo": 1}, timeout=REQUEST_TIMEOUT)
        if response.status_code == 200:
            print(f"Ordine effettuato con successo per {email}")
            increment_success_count()
        else:
            print(f"Errore durante l'ordine per {email}. Status code: {response.status_code}")
    except requests.exceptions.RequestException as e:
        print(f"Errore nella richiesta per l'effettuazione dell'ordine: {e}")

### Funzione per eseguire tutti i test per una specifica email ###
def run_tests_for_email(email):
    print(f"\nEsecuzione dei test per {email}...")

    print("\n--- Test Autentica ---")
    test_autentica(email)

    print("\n--- Test Crea Customer ---")
    test_crea_customer(email)

    print("\n--- Test Modifica Info Customer ---")
    test_modifica_info(email)

    print("\n--- Test Recupera Indirizzi ---")
    test_recupera_indirizzi(email)

    print("\n--- Test Rimuovi Indirizzo ---")
    test_remove_indirizzo(email)

    print("\n--- Test Recupera Ordini ---")
    test_recupera_ordini(email)

    print("\n--- Test Annulla Ordine ---")
    test_annulla_ordine(email)

    print("\n--- Test Recupera Prodotti ---")
    test_recupera_prodotti(email)

    print("\n--- Test Aggiungi Prodotto al Carrello ---")
    test_aggiungi_prodotto_al_carrello(email)

    print("\n--- Test Rimuovi Prodotto dal Carrello ---")
    test_remove_prodotto_carrello(email)

    print("\n--- Test Effettua Ordine ---")
    test_ordina(email)

### Funzione principale per eseguire tutti i test su più email contemporaneamente ###
def run_all_tests_concurrently():
    print("Esecuzione dei test automatici con più email...\n")

    with concurrent.futures.ThreadPoolExecutor(max_workers=len(email_list)) as executor:
        futures = [executor.submit(run_tests_for_email, email) for email in email_list]
        concurrent.futures.wait(futures)

    # Stampa dei risultati finali
    print(f"\nTest automatici completati: {success_count} su {total_tests} test sono andati a buon fine.")

if __name__ == "__main__":
    run_all_tests_concurrently()
