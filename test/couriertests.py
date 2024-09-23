import requests
import random
import json

# Dati di configurazione
HOST = "127.0.0.1"
PORT = 5003
BASE_URL = f"http://{HOST}:{PORT}"

# Lista di PIVA per i trasportatori esistenti nel sistema
piva_list = ["32132132132", "45645645645", "78978978978"]

# Variabile per contare i test riusciti
success_count = 0
total_tests = 3 * len(piva_list)  # Numero totale di test da eseguire

# Funzione per ottenere una PIVA casuale dal sistema
def get_random_piva():
    return random.choice(piva_list)

### Test per ciascuna rotta ###

# 1. Test autentica trasportatore
def test_autentica_trasportatore(piva):
    global success_count
    try:
        response = requests.post(f"{BASE_URL}/autentica/{piva}")
        if response.status_code == 200:
            print(f"Autenticazione avvenuta con successo per PIVA {piva}")
            success_count += 1  # Incrementa il contatore in caso di successo
        elif response.status_code == 401:
            print(f"Autenticazione fallita per PIVA {piva}: Non autorizzato.")
        else:
            print(f"Errore durante l'autenticazione per PIVA {piva}. Status code: {response.status_code}")
    except requests.exceptions.RequestException as e:
        print(f"Errore di connessione durante il test di autenticazione per PIVA {piva}: {e}")

# 2. Test recupera ordini
def test_get_ordini():
    global success_count
    try:
        response = requests.get(f"{BASE_URL}/ordini/")
        if response.status_code == 200:
            print("Ordini recuperati con successo:")
            print(response.text)
            success_count += 1  # Incrementa il contatore in caso di successo
            return None
        elif response.status_code == 500:
            print("Errore interno nel recupero degli ordini.")
        else:
            print(f"Errore nel recupero degli ordini. Status code: {response.status_code}")
    except requests.exceptions.RequestException as e:
        print(f"Errore di connessione durante il test di recupero ordini: {e}")
    return None  # In caso di errore, ritorna None

# 3. Test prendere in carico un ordine
def test_accetta_ordine(piva, ordine_id, corriere_id):
    global success_count
    try:
        response = requests.post(f"{BASE_URL}/accetta/{piva}/{ordine_id}/{corriere_id}")
        if response.status_code == 200:
            print(f"Ordine {ordine_id} preso in carico con successo per PIVA {piva}")
            success_count += 1  # Incrementa il contatore in caso di successo
        else:
            print(f"Errore nel prendere in carico l'ordine {ordine_id}. Status code: {response.status_code}")
    except requests.exceptions.RequestException as e:
        print(f"Errore di connessione durante il test di presa in carico ordine {ordine_id}: {e}")

### Funzione principale per eseguire tutti i test ###
def run_all_tests():
    print("Esecuzione dei test automatici...\n")

    # Esegui il test per ogni PIVA
    for piva in piva_list:
        print(f"\n--- Test Autentica Trasportatore (PIVA: {piva}) ---")
        test_autentica_trasportatore(piva)

        print("\n--- Test Recupera Ordini ---")
        ordini_data = test_get_ordini()
        
        test_accetta_ordine(piva, 5, 1)
        """
        if ordini_data and len(ordini_data) > 0:
            # Assumiamo che l'ID del corriere sia sempre valido (es. 1), e prendiamo il primo ordine disponibile
            ordine_id = ordini_data[0]['ID']  # Ottieni l'ID del primo ordine
            corriere_id = 1  # Può essere sostituito con un ID corriere valido se necessario

            print(f"\n--- Test Prendi in carico Ordine (ID Ordine: {ordine_id}, PIVA: {piva}) ---")
            test_accetta_ordine(piva, ordine_id, corriere_id)
        else:
            print("Nessun ordine disponibile da prendere in carico.") """

    # Stampa dei risultati finali
    print(f"\nTest automatici completati: {success_count} su {total_tests} test sono andati a buon fine.")

if __name__ == "__main__":
    run_all_tests()
