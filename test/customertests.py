import requests
import random
import concurrent.futures
import threading

# Dati di configurazione
HOST = "127.0.0.1"
PORT = 5001
BASE_URL = f"http://{HOST}:{PORT}"

# Lista di email che già esistono nel sistema
email_list = ["abc@abc.it", "cab@cab.it", "bca@bca.it"]

# Lock per gestire l'accesso concorrente al contatore
lock = threading.Lock()

# Variabile per contare i test riusciti
success_count = 0
total_tests = 5 * len(email_list)  # Numero totale di test da eseguire

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

def test_crea_customer(data, expected_status_code, expected_message):
    response = requests.post(f"{BASE_URL}/creaCustomer", json=data)
    assert response.status_code == expected_status_code, f"Expected {expected_status_code}, got {response.status_code}"
    assert response.text.strip() == expected_message, f"Expected message '{expected_message}', got '{response.text.strip()}'"

def test_dati_corretti():
    data = {
        "email": "test@example.com",
        "nome": "Mario",
        "cognome": "Rossi",
        "via": "Via Roma",
        "civico": 10,
        "cap": "00100",
        "city": "Roma",
        "stato": "Italia"
    }
    test_crea_customer(data, 201, "Customer created")

def test_modifica_info(data, expected_status_code, expected_message, email):
    response = requests.put(f"{BASE_URL}/modificaInfo/{email}", json=data)
    assert response.status_code == expected_status_code, f"Expected {expected_status_code}, got {response.status_code}"
    assert response.text.strip() == expected_message, f"Expected message '{expected_message}', got '{response.text.strip()}'"

# 2. Test modificaNome
def test_modifica_nome():
    data = {"nome": "NuovoNome"}  # Simula l'invio di un nuovo nome
    response = requests.post(f"{BASE_URL}/modificaNome", json=data)
    if response.status_code == 200:
        print(f"Nome modificato con successo")
        increment_success_count()  # Incrementa il contatore in caso di successo
    else:
        print(f"Errore durante la modifica del nome. Status code: {response.status_code}")

# 3. Test recupera prodotti
def test_recupera_prodotti():
    response = requests.get(f"{BASE_URL}/prodotti")
    if response.status_code == 200:
        print("Prodotti recuperati con successo:")
        # prodotti_disponibili = response.json()  # Memorizza i prodotti recuperati
        increment_success_count()  # Incrementa il contatore in caso di successo
    else:
        print(f"Errore nel recupero dei prodotti. Status code: {response.status_code}")

# 4. Test aggiungi prodotto al carrello
def test_aggiungi_prodotto_al_carrello(email):
    prodotto_id = random.randint(1, 7)  # Scegli un prodotto casuale
    quantita = random.randint(1, 5)  # Quantità casuale tra 1 e 5

    response = requests.post(f"{BASE_URL}/addToCarrello/{email}/{prodotto_id}/{quantita}")
    if response.status_code == 200:
        print(f"Prodotto aggiunto al carrello per l'utente {email} con quantità {quantita}")
        increment_success_count()  # Incrementa il contatore in caso di successo
    else:
        print(f"Errore nell'aggiunta del prodotto al carrello per {email}. Status code: {response.status_code}")

# 5. Test visualizza carrello
def test_visualizza_carrello(email):
    response = requests.get(f"{BASE_URL}/{email}/carrello")
    if response.status_code == 200:
        print(f"Carrello recuperato per {email}:")
        print(response.text)
        increment_success_count()  # Incrementa il contatore in caso di successo
    else:
        print(f"Errore nel recupero del carrello per {email}. Status code: {response.status_code}")

def test_get_indirizzi_corretti(email):
    response = requests.get(f"{BASE_URL}/{email}/indirizzi")
    if response.status_code == 200:
        print(f"Indirizzi recuperati per {email}:")
        print(response.text)
        increment_success_count()  # Incrementa il contatore in caso di successo
    else:
        print(f"Errore nel recupero degli indirizzi per {email}. Status code: {response.status_code}")

# 6. Test effettua ordine
def test_ordina(email):
    response = requests.post(f"{BASE_URL}/ordina/{email}")
    if response.status_code == 200:
        print(f"Ordine effettuato con successo per {email}")
        increment_success_count()  # Incrementa il contatore in caso di successo
    else:
        print(f"Errore durante l'ordine per {email}. Status code: {response.status_code}")

### Funzione per eseguire tutti i test per una specifica email ###
def run_tests_for_email(email):
    print(f"\nEsecuzione dei test per {email}...")

    #test_dati_corretti()

    print("\n--- Test Autentica ---")
    test_autentica(email)

    print("\n--- Test Modifica Nome ---")
    #test_modifica_nome()

    print("\n--- Test Recupera Prodotti ---")
    test_recupera_prodotti()

    print("\n--- Test Aggiungi Prodotto al Carrello ---")
    test_aggiungi_prodotto_al_carrello(email)

    print("\n--- Test Visualizza Carrello ---")
    test_visualizza_carrello(email)

    print("\n--- Test Visualizza Indirizzi ---")
    test_get_indirizzi_corretti(email)

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
