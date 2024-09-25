import requests
import threading

# Configurazione del server
HOST = "127.0.0.1"
PORT = 5001
BASE_URL = f"http://{HOST}:{PORT}"

# Lock per gestire l'accesso concorrente al contatore
lock = threading.Lock()
success_count = 0

# Funzione per aggiornare il contatore dei successi in modo thread-safe
def increment_success_count():
    global success_count
    with lock:
        success_count += 1

# Funzione per testare la creazione di un cliente
def test_crea_customer(data, expected_status_code, expected_message):
    response = requests.put(f"{BASE_URL}/creaCustomer", json=data)
    assert response.status_code == expected_status_code, f"Expected {expected_status_code}, got {response.status_code}"
    assert response.text.strip() == expected_message, f"Expected message '{expected_message}', got '{response.text.strip()}'"

# 1. Test con dati corretti
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

# 2. Test con email mancante
def test_email_mancante():
    data = {
        "nome": "Mario",
        "cognome": "Rossi",
        "via": "Via Roma",
        "civico": 10,
        "cap": "00100",
        "city": "Roma",
        "stato": "Italia"
    }
    test_crea_customer(data, 400, "Email not provided")

# 3. Test con nome mancante
def test_nome_mancante():
    data = {
        "email": "test@example.com",
        "cognome": "Rossi",
        "via": "Via Roma",
        "civico": 10,
        "cap": "00100",
        "city": "Roma",
        "stato": "Italia"
    }
    test_crea_customer(data, 400, "Name not provided")

# 4. Test con cognome mancante
def test_cognome_mancante():
    data = {
        "email": "test@example.com",
        "nome": "Mario",
        "via": "Via Roma",
        "civico": 10,
        "cap": "00100",
        "city": "Roma",
        "stato": "Italia"
    }
    test_crea_customer(data, 400, "Surname not provided")

# 5. Test con via mancante
def test_via_mancante():
    data = {
        "email": "test@example.com",
        "nome": "Mario",
        "cognome": "Rossi",
        "civico": 10,
        "cap": "00100",
        "city": "Roma",
        "stato": "Italia"
    }
    test_crea_customer(data, 400, "Via not provided")

# 6. Test con civico non numerico
def test_civico_non_numerico():
    data = {
        "email": "test@example.com",
        "nome": "Mario",
        "cognome": "Rossi",
        "via": "Via Roma",
        "civico": "dieci",
        "cap": "00100",
        "city": "Roma",
        "stato": "Italia"
    }
    test_crea_customer(data, 400, "Civico not provided or not a number")

# 7. Test con CAP errato
def test_cap_errato():
    data = {
        "email": "test@example.com",
        "nome": "Mario",
        "cognome": "Rossi",
        "via": "Via Roma",
        "civico": 10,
        "cap": "123",
        "city": "Roma",
        "stato": "Italia"
    }
    test_crea_customer(data, 400, "CAP must be a string of 5 numbers")

# 8. Test con stato troppo lungo
def test_stato_troppo_lungo():
    data = {
        "email": "test@example.com",
        "nome": "Mario",
        "cognome": "Rossi",
        "via": "Via Roma",
        "civico": 10,
        "cap": "00100",
        "city": "Roma",
        "stato": "Questo è un stato con una lunghezza maggiore di cinquanta caratteri"
    }
    test_crea_customer(data, 400, "State length is above 50 characters")

# 9. Test con errore nella creazione del cliente
def test_errore_creazione():
    global success_count
    original_success_count = success_count
    # Simula un errore nella creazione (ipoteticamente, puoi fare mock della funzione `crea`)
    # Assumendo che la funzione creaCustomer sia stata implementata per gestire questo
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
    # Simula una risposta da parte del server che indica un errore
    test_crea_customer(data, 401, "Failed to create the Customer")

# Esecuzione dei test
if __name__ == "__main__":
    test_dati_corretti()
    test_email_mancante()
    test_nome_mancante()
    test_cognome_mancante()
    test_via_mancante()
    test_civico_non_numerico()
    test_cap_errato()
    test_stato_troppo_lungo()
    test_errore_creazione()
    
    print(f"Test completati: {success_count} test andati a buon fine.")
