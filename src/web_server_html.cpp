
#include "web_server_html.h"

AsyncWebServer server(80);
// Global instance of the SI4735 radio object
extern SI4735 si4735;

// Variabilă globală pentru volumul curent (presupunem că volumul este între 0 și 100)
int currentVolume = 50;

// Funcție pentru a împărți un șir pe baza unui delimitator
void splitString(const String &str, char delimiter, std::vector<String> &result) {
    size_t start = 0;
    size_t end = str.indexOf(delimiter);
    while (end != -1) {
        result.push_back(str.substring(start, end));
        start = end + 1;
        end = str.indexOf(delimiter, start);
    }
    result.push_back(str.substring(start));
}

// Endpoint pentru CSV
void handleGetCSVData(AsyncWebServerRequest *request) {
    File file = LittleFS.open("/Radio_fm/FM_Bucuresti.csv", "r");
    if (!file) {
        request->send(500, "text/plain", "Eroare la deschiderea fișierului CSV.");
        return;
    }

    DynamicJsonDocument doc(4096);
    JsonArray data = doc.to<JsonArray>();

    while (file.available()) {
        String line = file.readStringUntil('\n');
        line.trim();
        if (line.length() > 0) {
            std::vector<String> values;
            splitString(line, ',', values);

            JsonArray row = data.createNestedArray();
            for (const auto &value : values) {
                row.add(value);
            }
        }
    }
    file.close();

    String jsonData;
    serializeJson(doc, jsonData);
    request->send(200, "application/json", jsonData);
}

// Endpoint pentru volume
void handleSetVolume(AsyncWebServerRequest *request) {
    // Variabilă statică pentru a reține volumul curent
    static int currentVolume = 30; // Volum inițial setat la 30 (aproximativ jumătatea scalei)

    if (request->hasParam("volume")) {
        String volumeParam = request->getParam("volume")->value();
        int volumeValue = volumeParam.toInt();

        // Mapăm gama 1-100 la 1-63
        int mappedVolume = map(volumeValue, 1, 100, 1, 63);

        if (mappedVolume >= 1 && mappedVolume <= 63) {
            currentVolume = mappedVolume;
            si4735.setVolume(currentVolume); // Setăm volumul pe SI4735
            Serial.println("Volum setat la: " + String(currentVolume));
            request->send(200, "text/plain", "Volumul a fost setat la: " + String(volumeValue) + " (mapat la " + String(mappedVolume) + ").");
        } else {
            request->send(400, "text/plain", "Valoare volum invalidă.");
        }
    } else if (request->hasParam("action")) {
        String action = request->getParam("action")->value();

        // Logică pentru Volume Up
        if (action == "up") {
            if (currentVolume < 63) {
                currentVolume++;
                si4735.volumeUp();
                Serial.println("Volum crescut la: " + String(currentVolume));
                request->send(200, "text/plain", "Volumul a fost crescut la: " + String(currentVolume));
            } else {
                request->send(400, "text/plain", "Volumul este deja la maxim.");
            }
        }
        // Logică pentru Volume Down
        else if (action == "down") {
            if (currentVolume > 1) {
                currentVolume--;
                si4735.volumeDown();
                Serial.println("Volum scăzut la: " + String(currentVolume));
                request->send(200, "text/plain", "Volumul a fost scăzut la: " + String(currentVolume));
            } else {
                request->send(400, "text/plain", "Volumul este deja la minim.");
            }
        } else {
            request->send(400, "text/plain", "Acțiune invalidă pentru volum.");
        }
    } else {
        request->send(400, "text/plain", "Parametru 'volume' sau 'action' lipsă.");
    }
}

// Endpoint pentru setarea frecvenței
void handleSetFrequency(AsyncWebServerRequest *request) {
    if (request->hasParam("freq")) {
        String frequency = request->getParam("freq")->value();
        Serial.println("Frecvență selectată: " + frequency);

        // Convertim frecvența din MHz în kHz
        float selectedFrequency = frequency.toFloat();
        if (selectedFrequency > 0) {
            int currentFrequency = selectedFrequency * 100;  // Convertim în kHz

            // Configurăm radio-ul la frecvența selectată
            si4735.setFrequency(currentFrequency);  // Setăm direct frecvența în kHz
            si4735.setRdsConfig(1, 3, 3, 3, 3);     // Activăm RDS (dacă este pe FM)
            si4735.setGpioCtl(1, 0, 0);             // Unmute radio
            si4735.setGpio(1, 0, 0);                // Unmute radio

            Serial.println("Radio setat pe frecvența: " + String(currentFrequency) + " kHz");
            request->send(200, "text/plain", "Frecvența a fost setată pe radio.");
        } else {
            request->send(400, "text/plain", "Frecvență invalidă.");
        }
    } else {
        request->send(400, "text/plain", "Parametru 'freq' lipsă.");
    }
}

// Endpoint pentru mute/unmute
void handleToggleMute(AsyncWebServerRequest *request) {
    // Flag static pentru a păstra starea între apeluri
    static bool muteState = false;

    // Inversăm starea muteState
    muteState = !muteState;

    // Aplicăm schimbarea pe baza stării actuale
    if (muteState) {
        si4735.setGpio(0, 0, 0);  // Mute radio
        Serial.println("Radio Muted");
    } else {
        si4735.setGpio(1, 0, 0);  // Unmute radio
        Serial.println("Radio Unmuted");
    }

    // Construim un răspuns pentru client
    String response = muteState ? "Radio Muted" : "Radio Unmuted";
    request->send(200, "text/plain", response);

    // Log pentru debugging
    Serial.println("Mute/Unmute toggled via Web Interface.");
}

// Servește fișierele statice (CSS, JS) din LittleFS
void serveStaticFiles() {
  server.serveStatic("/Web_server_radio_fm/static", LittleFS, "/Web_server_radio_fm/static");
  Serial.println("Fișierele statice sunt servite.");
}

// Endpoint pentru a salva datele în fișierul CSV
void handleSaveCSVData(AsyncWebServerRequest *request) {
    if (request->hasParam("radioName", true) && request->hasParam("radioFrequency", true) && request->hasParam("radioType", true)) {
        // Obținem datele din formular
        String radioName = request->getParam("radioName", true)->value();
        String radioFrequency = request->getParam("radioFrequency", true)->value();
        String radioType = request->getParam("radioType", true)->value();

        // Deschidem fișierul CSV în modul append
        File file = LittleFS.open("/Radio_fm/FM_Bucuresti.csv", "a");
        if (!file) {
            request->send(500, "text/plain", "Eroare la deschiderea fișierului CSV.");
            return;
        }

        // Scriem datele în fișierul CSV
        file.print(radioName);
        file.print(",");
        file.print(radioFrequency);
        file.print(",");
        file.print(radioType);
        file.println(); // Adăugăm un salt de linie pentru a începe un nou rând
        file.close(); // Închidem fișierul

        // Răspuns către client
        request->send(200, "text/plain", "Postul FM a fost adăugat cu succes.");
    } else {
        request->send(400, "text/plain", "Parametrii lipsă.");
    }
}

// Endpoint pentru a salva datele în fișierul CSV pentru WIFI
void handleSaveCSVData_wifi(AsyncWebServerRequest *request) {
    if (request->hasParam("apartment", true) && request->hasParam("wifiPassword", true)) {
        String apartment = request->getParam("apartment", true)->value();
        String wifiPassword = request->getParam("wifiPassword", true)->value();

        // Deschide fișierul în modul de citire pentru a verifica ultima linie
        File file = LittleFS.open("/wifi/pass.csv", FILE_READ);
        bool endsWithNewline = false;

        if (file) {
            file.seek(file.size() - 1); // Mergi la finalul fișierului
            if (file.read() == '\n') { // Verifică dacă ultima linie se termină cu '\n'
                endsWithNewline = true;
            }
            file.close();
        }

        // Deschide fișierul în modul de scriere pentru a adăuga date
        file = LittleFS.open("/wifi/pass.csv", FILE_APPEND);
        if (file) {
            // Adaugă un newline dacă fișierul nu se termină cu unul
            if (!endsWithNewline) {
                file.print("\n");
            }
            // Scrie datele noi
            file.printf("%s,%s\n", apartment.c_str(), wifiPassword.c_str());
            file.close();
            request->send(200, "text/plain", "Datele au fost salvate cu succes!");
        } else {
            request->send(500, "text/plain", "Eroare la deschiderea fișierului CSV");
        }
    } else {
        request->send(400, "text/plain", "Parametri lipsă");
    }
}

// Endpoint pentru afișarea statusului tunerului
void handleRadioStatus(AsyncWebServerRequest *request) {
    // Obține statusul radio și calitatea semnalului
    si4735.getStatus();
    si4735.getCurrentReceivedSignalQuality();

    // Construiește răspunsul de bază
    String response;
    response += "You are tuned on ";
    if (si4735.isCurrentTuneFM()) {
        response += String(si4735.getFrequency() / 100.0, 2) + " MHz ";
        response += si4735.getCurrentPilot() ? "STEREO" : "MONO";
        response += " [SNR: " + String(si4735.getCurrentSNR()) + " dB, RSSI: " + String(si4735.getCurrentRSSI()) + " dBuV]";
    } else {
        response += String(si4735.getFrequency()) + " kHz";
    }
    response += " [Status: ";
    response += si4735.isCurrentTuneFM() ? "FM" : "AM";
    response += "]";

    // Adaugă mesajul RDS (dacă este disponibil)
    si4735.getRdsStatus();
    if (si4735.getRdsReceived() && si4735.getRdsSync() && si4735.getRdsSyncFound()) {
        const char* rdsMessage = si4735.getRdsText0A(); // Obține mesajul RDS
        if (rdsMessage != nullptr && strlen(rdsMessage) > 0) {
            response += " [RDS: " + String(rdsMessage) + "]";
        }
    }

    // Adaugă ora UTC (dacă este disponibilă)
    if (si4735.getNumRdsFifoUsed() > 0) {
        char* utcTime = si4735.getRdsDateTime(); // Obține timpul UTC
        if (utcTime != nullptr) {
            char timeOnly[6]; // Buffer pentru ora (HH:MM)
            strncpy(timeOnly, utcTime + 11, 5); // Extrage doar partea de timp (HH:MM)
            timeOnly[5] = '\0'; // Adaugă terminatorul de șir
            response += " [UTC: " + String(timeOnly) + "]";
        }
    }

    // Trimite răspunsul
    request->send(200, "text/plain", response);
}

// Inițializarea serverului web
void Server_ESP_32_Control_FM_WEB() {
    if (!LittleFS.begin()) {
        Serial.println("Eroare la inițializarea LittleFS!");
        return;
    }

    // Servește fișierele statice
    serveStaticFiles();
    

    // Endpoint pentru statusul tunerului
    server.on("/getStatus", HTTP_GET, handleRadioStatus);

    // Endpoint pentru salvarea datelor Wi-Fi
    server.on("/saveCSVData_wifi", HTTP_POST, handleSaveCSVData_wifi);

    // Endpoint pentru a salva datele în fișierul CSV
    server.on("/saveCSVData", HTTP_POST, handleSaveCSVData);

    // Endpoint pentru CSV
    server.on("/getCSVData", HTTP_GET, handleGetCSVData);

    // Endpoint pentru setarea frecvenței
    server.on("/setFrequency", HTTP_GET, handleSetFrequency);

    // Endpoint pentru setarea volumului
    server.on("/setVolume", HTTP_GET, handleSetVolume);

    // Endpoint pentru mute/unmute
    server.on("/toggleMute", HTTP_GET, handleToggleMute);

    // Servește pagina principală
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(LittleFS, "/Web_server_radio_fm/templates/index.html", "text/html");
    });

    // Pornește serverul
    server.begin();
    Serial.println("Serverul web a fost pornit.");
}

