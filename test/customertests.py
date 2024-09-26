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
total_tests = 6 * len(email_list)  # Numero totale di test da eseguire

# Funzione per aggiornare il contatore dei successi in modo thread-safe
def increment_success_count():
    global success_count
    with lock:
        success_count += 1

### Test per ciascuna rotta ###

# 1. Test autentica
def test_autentica(email):
    response = requests.get(f"{BASE_URL}/autentica/{email}")
    if response.status_code == 200:
        print(f"Autenticazione avvenuta con successo per {email}")
        increment_success_count()  # Incrementa il contatore in caso di successo
    else:
        print(f"Errore durante l'autenticazione per {email}. Status code: {response.status_code}")

# 2. Test recupera indirizzi
def test_recupera_indirizzi(email):
    response = requests.get(f"{BASE_URL}/{email}/indirizzi/")
    if response.status_code == 200:
        print(f"Indirizzi recuperati con successo per {email}")
        increment_success_count()  # Incrementa il contatore in caso di successo
    else:
        print(f"Errore durante il recupero degli indirizzi per {email}. Status code: {response.status_code}")

# 3. Test recupera prodotti
def test_recupera_prodotti(email):
    response = requests.get(f"{BASE_URL}/{email}/prodotti")
    if response.status_code == 200:
        print("Prodotti recuperati con successo:")
        increment_success_count()  # Incrementa il contatore in caso di successo
    else:
        print(f"Errore nel recupero dei prodotti. Status code: {response.status_code}")

# 4. Test aggiungi prodotto al carrello
def test_aggiungi_prodotto_al_carrello(email):
    response = requests.put(f"{BASE_URL}/{email}/carrello/", json={"quantita": 1, "IDprodotto": 1})
    if response.status_code == 200:
        print(f"Prodotto aggiunto al carrello per l'utente {email} con quantità {quantita}")
        increment_success_count()  # Incrementa il contatore in caso di successo
    else:
        print(f"Errore nell'aggiunta del prodotto al carrello per {email}. Status code: {response.status_code}")

# 5. Test visualizza carrello
def test_visualizza_carrello(email):
    response = requests.get(f"{BASE_URL}/{email}/carrello/")
    if response.status_code == 200:
        print(f"Carrello recuperato per {email}:")
        print(response.text)
        increment_success_count()  # Incrementa il contatore in caso di successo
    else:
        print(f"Errore nel recupero del carrello per {email}. Status code: {response.status_code}")

# 6. Test effettua ordine
def test_ordina(email):
    response = requests.put(f"{BASE_URL}/{email}/ordini/", json={"pagamento": 'bancomat', "indirizzo": 1})
    if response.status_code == 200:
        print(f"Ordine effettuato con successo per {email}")
        increment_success_count()  # Incrementa il contatore in caso di successo
    else:
        print(f"Errore durante l'ordine per {email}. Status code: {response.status_code}")

### Funzione per eseguire tutti i test per una specifica email ###
def run_tests_for_email(email):
    print(f"\nEsecuzione dei test per {email}...")

    print("\n--- Test Autentica ---")
    test_autentica(email)

    print("\n--- Test Recupera Indirizzi ---")
    test_recupera_indirizzi(email)

    print("\n--- Test Recupera Prodotti ---")
    test_recupera_prodotti(email)

    print("\n--- Test Aggiungi Prodotto al Carrello ---")
    test_aggiungi_prodotto_al_carrello(email)

    print("\n--- Test Visualizza Carrello ---")
    test_visualizza_carrello(email)

    print("\n--- Test Effettua Ordine ---")
    test_ordina(email)

### Funzione principale per eseguire tutti i test su più email contemporaneamente ###
def run_all_tests_concurrently():
    print("Esecuzione dei test automatici con più email...\n")

    with concurrent.futures.ThreadPoolExecutor(max_workers=len(email_list)) as executor:
        # Lancia i test per ogni email in modo concorrente
        futures = [executor.submit(run_tests_for_email, email) for email in email_list]
        
        # Attendi il completamento di tutti i thread
        concurrent.futures.wait(futures)

    # Stampa dei risultati finali
    print(f"\nTest automatici completati: {success_count} su {total_tests} test sono andati a buon fine.")

if __name__ == "__main__":
    run_all_tests_concurrently()
