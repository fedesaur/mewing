import requests
import json

# Configurazione del server
HOST = "127.0.0.1"
PORT = 5005
BASE_URL = f"http://{HOST}:{PORT}"

# Funzione per autenticare un fornitore
def test_autentica_fornitore(email):
    url = f"{BASE_URL}/autentica/{email}"
    response = requests.get(url)
    if response.status_code == 200:
        print(f"Autenticazione avvenuta con successo per {email}")
    else:
        print(f"Errore nell'autenticazione per {email}. Status Code: {response.status_code}")

# Funzione per recuperare prodotti
def test_get_prodotti(email):
    url = f"{BASE_URL}/{email}/prodotti/"
    response = requests.get(url)
    if response.status_code == 200:
        print(f"Prodotti recuperati per {email}: {response.text}")
    else:
        print(f"Errore nel recupero prodotti per {email}. Status Code: {response.status_code}")

# Funzione per aggiungere un prodotto
def test_aggiungi_prodotto(email, nomeProdotto, descrizioneProdotto, prezzoProdotto):
    url = f"{BASE_URL}/{email}/prodotti/"
    data = {
        "nomeProdotto": nomeProdotto,
        "descrizioneProdotto": descrizioneProdotto,
        "prezzoProdotto": prezzoProdotto
    }
    response = requests.put(url, json=data)
    if response.status_code == 201:
        print(f"Prodotto aggiunto con successo per {email}")
    else:
        print(f"Errore nell'aggiunta del prodotto per {email}. Status Code: {response.status_code}")

# Funzione per modificare un prodotto
def test_modifica_prodotto(email, idProdotto, nomeProdotto, descrizioneProdotto, prezzoProdotto):
    url = f"{BASE_URL}/{email}/prodotti/{idProdotto}"
    data = {
        "nomeProdotto": nomeProdotto,
        "descrizioneProdotto": descrizioneProdotto,
        "prezzoProdotto": prezzoProdotto
    }
    response = requests.post(url, json=data)
    if response.status_code == 201:
        print(f"Prodotto modificato con successo per {email}")
    else:
        print(f"Errore nella modifica del prodotto per {email}. Status Code: {response.status_code}")

# Funzione per eliminare un prodotto
def test_elimina_prodotto(email, idProdotto):
    url = f"{BASE_URL}/{email}/prodotti/{idProdotto}"
    response = requests.delete(url)
    if response.status_code == 200:
        print(f"Prodotto eliminato con successo per {email}")
    else:
        print(f"Errore nell'eliminazione del prodotto per {email}. Status Code: {response.status_code}")

# Funzione per modificare informazioni del fornitore
def test_modifica_info(email, nome, IVA, telefono):
    url = f"{BASE_URL}/{email}/"
    data = {
        "nome": nome,
        "IVA": IVA,
        "telefono": telefono
    }
    response = requests.post(url, json=data)
    if response.status_code == 201:
        print(f"Informazioni modificate con successo per {email}")
    else:
        print(f"Errore nella modifica delle informazioni per {email}. Status Code: {response.status_code}")

# Funzione principale per eseguire tutti i test
def run_tests():
    email = "prova1@prova1.it"
    
    print("\n--- Test Autenticazione ---")
    test_autentica_fornitore(email)
    
    print("\n--- Test Recupera Prodotti ---")
    test_get_prodotti(email)
    
    print("\n--- Test Aggiungi Prodotto ---")
    test_aggiungi_prodotto(email, "NuovoProdotto", "Descrizione del prodotto", 10.50)
    
    print("\n--- Test Modifica Prodotto ---")
    test_modifica_prodotto(email, 1, "ProdottoModificato", "Nuova descrizione", 15.99)
    
    print("\n--- Test Elimina Prodotto ---")
    test_elimina_prodotto(email, 1)
    
    print("\n--- Test Modifica Informazioni Fornitore ---")
    test_modifica_info(email, "NuovoNome", "12312312312", "1234567890")

if __name__ == "__main__":
    run_tests()
