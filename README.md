# 📻 ESP32 Radio FM Web Server

Un proiect complex ce transformă un ESP32 într-un radio FM inteligent, controlabil printr-o interfață web modernă și afișaj TFT rotund, cu suport pentru control fizic (modul TM1638), conexiuni Wi-Fi și RDS.

---

## 🚀 Funcționalități principale

- 🎵 **Control radio FM** (frecvență, volum, mute/unmute, căutare stații).
- 🌐 **Server web integrat** pentru control de la distanță.
- 📊 **Afișaj TFT ST7789** cu ceas, vreme și informații RDS.
- 🔢 **Control fizic TM1638** cu butoane și display LED.
- 📡 **Conectare Wi-Fi automată** sau mod Hotspot.
- 🗂️ **Gestionare posturi radio și parole Wi-Fi** din fișiere CSV.
- 🌍 **Afișare date NTP** pentru sincronizare oră globală.

---

📄 Pagina web
Titlu: Radio FM ESP32
Componente:
radioStatusContainer: Afișează starea actuală a radio-ului.
confirmationMessage: Mesaj de confirmare pentru acțiuni.
Clase CSS importante: container, main-content, control-panel, form-panel, etc.


🔌 API-uri disponibile
Endpoint	Descriere
/getCSVData	Returnează datele posturilor FM.
/toggleMute	Activează/Dezactivează sunetul.
/saveCSVData	Salvează posturi FM noi.
/saveCSVData_wifi	Salvează credențiale Wi-Fi.
/getStatus	Afișează starea radio-ului.


🛠️ Librării utilizate
plaintext
Copiază
Editează
ArduinoJson, ESPAsyncWebServer, TFT_eSPI, TM1638plus, SI4735, RDS, WiFi, LittleFS, AsyncTCP, TimeLib, HTTPClient


🔄 Funcții cheie
🔹 Web (script.js):

loadCSVData()
sendFrequency()
toggleMute()
fetchRadioStatus()
🔹 ESP32:

Control prin serial, TM1638 și Web Server.
Management RDS și afișare pe TFT.
Monitorizare resurse și grafic CPU.

📊 Statistici
Metrică	Valoare
Fișiere totale	24
Linii de cod	3627
Funcții	189
Clase	0



🧑‍💻 Autor
Krux – Forensic Investigator & IoT Enthusiast
GitHub • Instagram


📝 Licență
Acest proiect este licențiat sub MIT License.