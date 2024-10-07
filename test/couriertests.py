import unittest
import requests
import random

# Configurazione di base
BASE_URL = "http://127.0.0.1:5003"

# Lista di PIVA esistenti
piva_list = ["32132132132", "45645645645", "78978978978"]

# Test class
class TestAPI(unittest.TestCase):

    # Funzione per ottenere una PIVA casuale
    def get_random_piva(self):
        return random.choice(piva_list)

    # Test autenticazione trasportatore
    def test_autentica_trasportatore(self):
        piva = self.get_random_piva()
        response = requests.get(f"{BASE_URL}/autentica/{piva}")
        if response.status_code == 200:
            self.assertEqual(response.text, "Autenticazione avvenuta con successo.")
        else:
            self.assertEqual(response.status_code, 401)

    # Test recupera ordini
    def test_get_ordini(self):
        response = requests.get(f"{BASE_URL}/{self.get_random_piva()}/ricerca/")
        self.assertEqual(response.status_code, 200)
        ordini_data = response.json()
        self.assertIsInstance(ordini_data, list)
        print(f"Ordini: {ordini_data}")

    # Test recupera corrieri
    def test_get_corrieri(self):
        piva = self.get_random_piva()
        response = requests.get(f"{BASE_URL}/{piva}/corrieri/")
        self.assertEqual(response.status_code, 200)
        corrieri_data = response.json()
        self.assertIsInstance(corrieri_data, list)
        print(f"Corrieri: {corrieri_data}")

    # Test accettazione ordine
    def test_accetta_ordine(self):
        piva = self.get_random_piva()
        ordine_id = 1  # Simuliamo l'ordine con ID 1
        corriere_id = 1  # Simuliamo il corriere con ID 1
        response = requests.put(f"{BASE_URL}/{piva}/ordini/", json={"ordineID": ordine_id, "corriereID": corriere_id})
        self.assertEqual(response.status_code, 200)
        print(f"Risposta: {response.text}")

    # Test consegna ordine
    def test_consegna_ordine(self):
        piva = self.get_random_piva()
        ordine_id = 1  # Simuliamo l'ordine con ID 1
        response = requests.post(f"{BASE_URL}/{piva}/ordini/{ordine_id}")
        self.assertEqual(response.status_code, 200)
        print(f"Risposta: {response.text}")

    # Test eliminazione corriere
    def test_delete_corriere(self):
        piva = self.get_random_piva()
        corriere_id = 1  # Simuliamo il corriere con ID 1
        response = requests.delete(f"{BASE_URL}/{piva}/corrieri/{corriere_id}")
        self.assertEqual(response.status_code, 200)
        print(f"Risposta: {response.text}")

    # Test creazione trasportatore
    def test_crea_trasportatore(self):
        nuova_piva = "12312312312"  # Nuova PIVA
        response = requests.put(f"{BASE_URL}/autentica/", json={"piva": nuova_piva})
        self.assertEqual(response.status_code, 200)
        print(f"Risposta: {response.text}")

if __name__ == "__main__":
    unittest.main()
