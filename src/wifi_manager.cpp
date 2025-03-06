#include "wifi_manager.h"
#include "web_server_html.h" 
#include "TM1638_control.h"

// Funcție pentru conectarea la o rețea Wi-Fi
bool connectToWiFi(const char* ssid, const char* password) {
    WiFi.begin(ssid, password);
    Serial.printf("Conectare la rețeaua: %s\n", ssid);

    unsigned long startAttemptTime = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 10000) {
        delay(500);
        TM1638_displayWaitingDotsAndLEDs();  // Afișează puncte pentru a indica încercarea de conectare
        Serial.print(".");
    }

    if (WiFi.status() == WL_CONNECTED) {
        Serial.printf("\nConectat la Wi-Fi: %s\n", ssid);
        Serial.printf("IP: %s\n", WiFi.localIP().toString().c_str());
        return true;
    } else {
        Serial.println("\nEșec la conectarea Wi-Fi.");
        return false;
    }
}

// Funcție pentru salvarea credentialelor Wi-Fi
void saveWiFiCredentials(const char* ssid, const char* password) {
    File file = LittleFS.open(WIFI_CREDENTIALS_FILE, "a");
    if (file) {
        file.printf("%s,%s\n", ssid, password);
        file.close();
        Serial.println("Credentialele au fost salvate.");
    } else {
        Serial.println("Eroare la deschiderea fișierului pentru salvare.");
    }
}

// Funcție pentru scanarea rețelelor și conectare
void scanAndConnectWiFi() {
    if (!LittleFS.begin()) {
        Serial.println("Eroare la inițializarea LittleFS.");
        return;
    }

    File file = LittleFS.open(WIFI_CREDENTIALS_FILE, "r");
    if (!file) {
        Serial.println("Fișierul cu credentialele Wi-Fi nu există.");
        startHotspot();
        return;
    }

    int n = WiFi.scanNetworks();
    Serial.printf("Rețele detectate: %d\n", n);

    while (file.available()) {
        String line = file.readStringUntil('\n');
        line.trim(); // Elimină spațiile albe de la început și sfârșit
        if (line.length() == 0) continue; // Ignoră liniile goale

        int commaIndex = line.indexOf(',');
        if (commaIndex > 0) {
            String savedSSID = line.substring(0, commaIndex);
            String savedPassword = line.substring(commaIndex + 1);

            savedSSID.trim();
            savedPassword.trim();

            for (int i = 0; i < n; i++) {
                if (savedSSID == WiFi.SSID(i)) {
                    Serial.printf("Încerc să mă conectez la: %s\n", savedSSID.c_str());
                    if (connectToWiFi(savedSSID.c_str(), savedPassword.c_str())) {
                        file.close();
                        return;
                    }
                }
            }
        } else {
            Serial.println("Linie invalidă în fișierul CSV.");
        }
    }

    file.close();
    Serial.println("Niciun SSID salvat nu a fost găsit. Pornire hotspot...");
    startHotspot();
}

// Funcție pentru pornirea unui hotspot
void startHotspot() {
    // Pornire hotspot cu SSID și parolă implicite
    WiFi.softAP("ESP32_Radio_FM", "12345678");
    Serial.println("Hotspot activat.");
    Serial.printf("IP-ul hotspot-ului: %s\n", WiFi.softAPIP().toString().c_str());
    Serial.println("Serverul web este activ pe hotspot.");
}


// Funcție pentru a adăuga spații la sfârșitul unui șir pentru aliniere
String padEnd(String str, int length) {
  while (str.length() < length) {
    str += " ";  // Adaugă un spațiu la sfârșit
  }
  return str;
}

// Funcția pentru a citi și a arăta conținutul fișierului CSV sub forma unui tabel
void citesteFisierCSV() {
  // Inițializează LittleFS
  if (!LittleFS.begin()) {
    Serial.println("Eroare la inițializarea LittleFS!");
    return;
  }
  Serial.println("LittleFS inițializat cu succes!");

  // Deschide fișierul CSV pentru citire
  File file = LittleFS.open(FISIER_CSV, "r");
  if (!file) {
    Serial.println("Eroare la deschiderea fișierului!");
    return;
  }
  Serial.println("Fișier deschis cu succes!");

  // Afișează capul tabelului
  Serial.println("\n----------------------------------------------------");
  Serial.println("| SSID                |          Parola            |");
  Serial.println("----------------------------------------------------");

  int linesRead = 0;

  // Citește fișierul linie cu linie
  while (file.available()) {
    String line = file.readStringUntil('\n');
    line.trim();  // Elimină eventualele caractere de spațiu sau newline
    linesRead++;  // Incrementăm numărul de linii citite

    // Verifică dacă linia are suficiente coloane (2)
    int firstComma = line.indexOf(',');

    if (firstComma != -1) {
      String ssid = line.substring(0, firstComma);
      String parola = line.substring(firstComma + 1);

      // Alinierea coloanelor
      ssid = padEnd(ssid, 20);  // Aliniază SSID la 20 de caractere
      parola = padEnd(parola, 18);  // Aliniază Parola la 18 caractere

      // Afișează datele într-un format tabelar
      Serial.print("| ");
      Serial.print(ssid);
      Serial.print(" | ");
      Serial.print(parola);
      Serial.println(" |");
    }
  }

  // Închide fișierul
  file.close();
  Serial.println("----------------------------------------------------");
  Serial.print("Linii citite din CSV: ");
  Serial.println(linesRead);
}