import requests
import random

# Dati di configurazione
HOST = "127.0.0.1"
PORT = 5003
BASE_URL = f"http://{HOST}:{PORT}"

# Lista di PIVA per i trasportatori esistenti nel sistema
piva_list = ["32132132132", "45645645645", "78978978978"]

# Variabile per contare i test riusciti
success_count = 0
total_tests = 2 * len(piva_list)  # Numero totale di test da eseguire

# Funzione per ottenere una PIVA casuale dal sistema
def get_random_piva():
    return random.choice(piva_list)

### Test per ciascuna rotta ###

# 1. Test autentica trasportatore
def test_autentica_trasportatore():
    global success_count
    piva = get_random_piva()
    response = requests.post(f"{BASE_URL}/autentica/{piva}")
    if response.status_code == 200:
        print(f"Autenticazione avvenuta con successo per PIVA {piva}")
        success_count += 1  # Incrementa il contatore in caso di successo
    else:
        print(f"Errore durante l'autenticazione per PIVA {piva}. Status code: {response.status_code}")

# 2. Test recupera ordini
def test_get_ordini():
    global success_count
    response = requests.get(f"{BASE_URL}/ordini/")
    if response.status_code == 200:
        print("Ordini recuperati con successo:")
        print(response.text)
        success_count += 1  # Incrementa il contatore in caso di successo
    else:
        print(f"Errore nel recupero degli ordini. Status code: {response.status_code}")

### Funzione principale per eseguire tutti i test ###
def run_all_tests():
    print("Esecuzione dei test automatici...\n")

    # Esegui il test per ogni PIVA
    for piva in piva_list:
        print(f"\n--- Test Autentica Trasportatore (PIVA: {piva}) ---")
        test_autentica_trasportatore()

    print("\n--- Test Recupera Ordini ---")
    test_get_ordini()

    # Stampa dei risultati finali
    print(f"\nTest automatici completati: {success_count} su {total_tests} test sono andati a buon fine.")

if __name__ == "__main__":
    run_all_tests()
