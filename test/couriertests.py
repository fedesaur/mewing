import requests
import random

# Configurazione di base
BASE_URL = "http://127.0.0.1:5003"

# Lista di PIVA esistenti
piva_list = ["32132132132"]


    # Funzione per ottenere una PIVA casuale
def get_random_piva():
    return random.choice(piva_list)

    # Test autenticazione trasportatore
def test_autentica_trasportatore():
    piva = get_random_piva()
    response = requests.get(f"{BASE_URL}/autentica/{piva}")
    if response.status_code == 200:
        print(f"Autenticazione avvenuta con successo per {piva}")
    else:
        print(f"Errore durante l'autenticazione per {piva}. Status code: {response.status_code}")

    # Test recupera ordini
def test_get_ordini():
    piva = get_random_piva()
    response = requests.get(f"{BASE_URL}/{piva}/ricerca/")
    ordini_data = response.text
    print(f"Ordini: {ordini_data}")

    # Test recupera corrieri
def test_get_corrieri():
    piva = get_random_piva()
    response = requests.get(f"{BASE_URL}/{piva}/corrieri/")
    corrieri_data = response.text
    print(f"Corrieri: {corrieri_data}")

def test_crea_corriere():
    piva = get_random_piva()
    response = requests.put(f"{BASE_URL}/{piva}/corrieri/", json={"nome": "Eugenio", "cognome": "Montale"})
    print(f"Risposta: {response.text}")

    # Test accettazione ordine
def test_accetta_ordine():
    piva = get_random_piva()
    ordine_id = 3 # Simuliamo l'ordine con ID 1
    corriere_id = 1  # Simuliamo il corriere con ID 1
    response = requests.put(f"{BASE_URL}/{piva}/ordini/", json={"ordine": ordine_id, "corriere": corriere_id})
    print(f"Risposta: {response.text}")

    # Test consegna ordine
def test_consegna_ordine():
    piva = get_random_piva()
    ordine_id = 3  # Simuliamo l'ordine con ID 1
    response = requests.post(f"{BASE_URL}/{piva}/ordini/{ordine_id}")
    print(f"Risposta: {response.text}")

"""
    # Test eliminazione corriere
def test_delete_corriere():
    piva = get_random_piva()
    corriere_id = 1  # Simuliamo il corriere con ID 1
    response = requests.delete(f"{BASE_URL}/{piva}/corrieri/{corriere_id}")
    print(f"Risposta: {response.text}")
"""
    # Test creazione trasportatore
def test_crea_trasportatore():
    nuova_piva = "12312314444"  # Nuova PIVA
    response = requests.put(f"{BASE_URL}/autentica/", json={"nome": "AmaZZon", "piva": "12312314444", "via" : "Via del Denaro", "civico": 7, "cap" : "77777", "city" : "Napoli", "stato" : "Montecarlo"})
    print(f"Risposta: {response.text}")

if __name__ == "__main__":
    test_autentica_trasportatore()
    test_get_ordini()
    test_crea_corriere()
    test_get_corrieri()
    test_accetta_ordine()
    #test_consegna_ordine()
    test_crea_trasportatore()
