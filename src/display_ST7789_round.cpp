// display_ST7789_round.cpp
#include "display_ST7789_round.h"

TFT_eSPI tft = TFT_eSPI();




// This next function will be called during decoding of the jpeg file to
// render each block to the TFT.  If you use a different TFT library
// you will need to adapt this function to suit.
bool tft_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t* bitmap)
{
   // Stop further decoding as image is running off bottom of screen
  if ( y >= tft.height() ) return 0;

  // This function will clip the image block rendering automatically at the TFT boundaries
  tft.pushImage(x, y, w, h, bitmap);

  // This might work instead if you adapt the sketch to use the Adafruit_GFX library
  // tft.drawRGBBitmap(x, y, bitmap, w, h);

  // Return 1 to decode next block
  return 1;
}
unsigned long lastDrawTime = 0;
const unsigned long drawInterval = 2000; // 2 secunde



// Funcție pentru a desena imagineasi afisarea imaginii ceasului//////////////////////////////////////////////////////
void updateDigitalClock(const char* timeStr) {
    const int centerX = TFT_WIDTH / 2;
    const int centerY = TFT_HEIGHT / 2;

    // Șterge zona ceasului digital
   // tft.fillRect(centerX - 50, centerY - 15, 100, 30, TFT_BLACK);

    // Desenează timpul digital
    tft.setTextDatum(MC_DATUM);  // Centrare text
    tft.setTextColor(TFT_ORANGE);
    tft.drawString(timeStr, centerX, centerY+10 , 6);  // Font 4 // Coborât cu 20 de pixeli+ 80
}

void initNTP() {
    const char* ntpServer = "time.google.com";
    const long gmtOffset_sec = 2 * 3600;
    const int daylightOffset_sec = 3600;

    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    Serial.println("NTP configurat.");
}

void getNTPTime(char* timeStr, size_t len) {
    initNTP();
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
        Serial.println("Eroare: Nu s-a putut obține timpul NTP.");
        strncpy(timeStr, "--:--", len);
        return;
    }

    snprintf(timeStr, len, "%02d:%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
}

void updateClockDisplay() {
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_ORANGE);
    clock_image(); 
    // Sincronizează timpul NTP la fiecare 5 minute
    static unsigned long lastSyncTime = 0;
    const unsigned long syncInterval = 5 * 60 * 1000;
    unsigned long currentMillis = millis();
    
    // Sincronizează timpul NTP la fiecare 5 minute
    if (currentMillis - lastSyncTime >= syncInterval) {
        char timeStr[9];
        getNTPTime(timeStr, sizeof(timeStr));
        lastSyncTime = currentMillis;  // Actualizează timpul ultimei sincronizări
    }

    // Actualizează ceasul digital folosind timpul local
    struct tm timeinfo;
    if (getLocalTime(&timeinfo)) {
        char timeStr[9];
        snprintf(timeStr, sizeof(timeStr), "%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min);
        updateDigitalClock(timeStr);  // Actualizează ceasul digital
    }
}

void clock_image() {
    unsigned long currentMillis = millis();

    if (currentMillis - lastDrawTime >= drawInterval) {
        // Afișează imaginea doar la fiecare 2 secunde
        lastDrawTime = currentMillis;

        // Codul pentru a desena imaginea
        tft.setTextColor(0xFFFF, 0x0000);
        tft.fillScreen(TFT_BLACK);
        tft.setSwapBytes(true);

        TJpgDec.setJpgScale(2);
        TJpgDec.setCallback(tft_output);
        tft.fillScreen(TFT_BLACK);

        uint32_t t = millis();
        uint16_t w = 0, h = 0;
        TJpgDec.getFsJpgSize(&w, &h, "/Radio_jpg/clock.jpg", LittleFS);
        //Serial.print("Width = ");
        //Serial.print(w);
        //Serial.print(", height = ");
        //Serial.println(h);

        uint16_t scaled_w = w / 2;
        uint16_t scaled_h = h / 2;
        int16_t x = (TFT_WIDTH - scaled_w) / 2;
        int16_t y = (TFT_HEIGHT - scaled_h+10) / 2; // +10 pentru a coborî imaginea

        TJpgDec.drawFsJpg(x, y, "/Radio_jpg/clock.jpg", LittleFS);

        //t = millis() - t;
        //Serial.print(t);
        Serial.println("Image drawing time");
    }
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


/////functii pentru afisarea imaginii wifi si a adresei IP a modulului wifi///////////////////////////////////////////
void displayWiFiIP() {
    // Șterge ecranul înainte de afișare (opțional, dacă vrei să fie curat)
    
    const int centerX = TFT_WIDTH / 2;  // Centrul orizontal
    const int centerY = TFT_HEIGHT / 2; // Centrul vertical


    wifi_image();
    // Setează stilul textului
    tft.setTextDatum(MC_DATUM);  // Centrare text
    tft.setTextColor(TFT_ORANGE);  // Text portocaliu, fără fundal

    // Obține adresa IP
    String ipAddress = WiFi.localIP().toString();

    // Afișează adresa IP pe centru
    tft.drawString(ipAddress, centerX, centerY+84, 4);  // Font 4
}

void wifi_image() {
    unsigned long currentMillis = millis();

    if (currentMillis - lastDrawTime >= drawInterval) {
        // Afișează imaginea doar la fiecare 2 secunde
        lastDrawTime = currentMillis;

        // Codul pentru a desena imaginea
        tft.setTextColor(0xFFFF, 0x0000);
        tft.fillScreen(TFT_BLACK);
        tft.setSwapBytes(true);

        TJpgDec.setJpgScale(2);
        TJpgDec.setCallback(tft_output);
        tft.fillScreen(TFT_BLACK);

        uint32_t t = millis();
        uint16_t w = 0, h = 0;
        TJpgDec.getFsJpgSize(&w, &h, "/Radio_jpg/wifi.jpg", LittleFS);
        //Serial.print("Width = ");
        //Serial.print(w);
        //Serial.print(", height = ");
        //Serial.println(h);

        uint16_t scaled_w = w / 2;
        uint16_t scaled_h = h / 2;
        int16_t x = (TFT_WIDTH - scaled_w) / 2;
        int16_t y = (TFT_HEIGHT - scaled_h) / 2;

        TJpgDec.drawFsJpg(x, y, "/Radio_jpg/wifi.jpg", LittleFS);

        //t = millis() - t;
        //Serial.print(t);
        //Serial.println(" ms");
        Serial.println("Image drawing WiFi");
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// functii pentru afisarea imaginii radio si a frecventei curente a postului de radio////////////////////////////////
void displayCurrentFrequency_ST7789() {
    if (si4735.isCurrentTuneFM()) {
        // Obține frecvența curentă în kHz și convertește în MHz
        float frequencyMHz = si4735.getFrequency() / 100.0;

        // Formatează textul pentru afișaj
        char displayText[10];
        snprintf(displayText, sizeof(displayText),"%04.1f", frequencyMHz); // Afișăm cu 1 zecimală

        // Configurare afișaj
        const int centerX = TFT_WIDTH / 2;  // Centrul orizontal
        const int centerY = TFT_HEIGHT / 2; // Centrul vertical

        // Șterge fundalul înainte de afișare (opțional)
        tft.fillScreen(TFT_BLACK);
        radio_image();

        // Setează stilul textului
        tft.setTextDatum(MC_DATUM);  // Centrare text
        tft.setTextColor(TFT_MAGENTA);  // Text alb, fără fundal

        // Afișează textul pe display
        tft.drawString(displayText, centerX-21, centerY+15, 4);  // Font 4


    } else {
        // Dacă nu există o frecvență FM curentă
        Serial.println("Nu există o frecvență FM curentă.");

        // Șterge ecranul și afișează un mesaj de eroare
        tft.fillScreen(TFT_BLACK);
        tft.setTextDatum(MC_DATUM);
        tft.setTextColor(TFT_RED);  // Text roșu pentru eroare
        tft.drawString("NO FM SIGNAL", TFT_WIDTH / 2, TFT_HEIGHT / 2, 4);
    }
}

void radio_image() {
    unsigned long currentMillis = millis();

    if (currentMillis - lastDrawTime >= drawInterval) {
        // Afișează imaginea doar la fiecare 2 secunde
        lastDrawTime = currentMillis;

        // Codul pentru a desena imaginea
        tft.setTextColor(0xFFFF, 0x0000);
        tft.fillScreen(TFT_BLACK);
        tft.setSwapBytes(true);

        TJpgDec.setJpgScale(2);
        TJpgDec.setCallback(tft_output);
        tft.fillScreen(TFT_BLACK);

        uint32_t t = millis();
        uint16_t w = 0, h = 0;
        TJpgDec.getFsJpgSize(&w, &h, "/Radio_jpg/radio.jpg", LittleFS);
        //Serial.print("Width = ");
        //Serial.print(w);
        //Serial.print(", height = ");
        //Serial.println(h);

        uint16_t scaled_w = w / 2;
        uint16_t scaled_h = h / 2;
        int16_t x = (TFT_WIDTH - scaled_w) / 2;
        int16_t y = (TFT_HEIGHT - scaled_h) / 2;

        TJpgDec.drawFsJpg(x, y, "/Radio_jpg/radio.jpg", LittleFS);

        //t = millis() - t;
        //Serial.print(t);
        //Serial.println(" ms");
        Serial.println("Image drawing Radio");
    }
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// Declarați variabilele globale
int centerX, centerY;       // Centrele pentru ecran

void initDisplay_analog() {
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);

    centerX = TFT_WIDTH / 2;  // Centrul orizontal
    centerY = TFT_HEIGHT / 2; // Centrul vertical

    // Desenare cadran ceas analog
    tft.fillCircle(centerX, centerY + 20, 118, TFT_GREEN);
    tft.fillCircle(centerX, centerY + 20, 110, TFT_BLACK);

    for (int i = 0; i < 360; i += 30) {
        float sx = cos((i - 90) * 0.0174532925);
        float sy = sin((i - 90) * 0.0174532925);
        uint16_t x0 = sx * 114 + centerX;
        uint16_t y0 = sy * 114 + centerY + 20;
        uint16_t x1 = sx * 100 + centerX;
        uint16_t y1 = sy * 100 + centerY + 20;
        tft.drawLine(x0, y0, x1, y1, TFT_GREEN);
    }

    for (int i = 0; i < 360; i += 6) {
        float sx = cos((i - 90) * 0.0174532925);
        float sy = sin((i - 90) * 0.0174532925);
        uint16_t x0 = sx * 102 + centerX;
        uint16_t y0 = sy * 102 + centerY + 20;
        tft.drawPixel(x0, y0, TFT_WHITE);
        if (i % 90 == 0) {
            tft.fillCircle(x0, y0, 2, TFT_WHITE);
        }
    }

    tft.fillCircle(centerX, centerY + 1, 3, TFT_WHITE);
}

void updateAnalogClock(uint8_t hh, uint8_t mm, uint8_t ss) {
    static uint16_t osx = centerX, osy = centerY;
    static uint16_t omx = centerX, omy = centerY;
    static uint16_t ohx = centerX, ohy = centerY;

    float sdeg = ss * 6;
    float mdeg = mm * 6 + sdeg * 0.01666667;
    float hdeg = (hh+1 + 23) % 24 * 30 + mdeg * 0.0833333;

    float sx = cos((sdeg - 90) * 0.0174532925);
    float sy = sin((sdeg - 90) * 0.0174532925);
    float mx = cos((mdeg - 90) * 0.0174532925);
    float my = sin((mdeg - 90) * 0.0174532925);
    float hx = cos((hdeg - 90) * 0.0174532925);
    float hy = sin((hdeg - 90) * 0.0174532925);

    tft.drawLine(osx, osy, centerX, centerY, TFT_BLACK);
    osx = sx * 90 + centerX;
    osy = sy * 90 + centerY;
    tft.drawLine(osx, osy, centerX, centerY, TFT_RED);

    tft.drawLine(omx, omy, centerX, centerY, TFT_BLACK);
    omx = mx * 84 + centerX;
    omy = my * 84 + centerY;
    tft.drawLine(omx, omy, centerX, centerY, TFT_WHITE);

    tft.drawLine(ohx, ohy, centerX, centerY, TFT_BLACK);
    ohx = hx * 62 + centerX;
    ohy = hy * 62 + centerY;
    tft.drawLine(ohx, ohy, centerX, centerY, TFT_WHITE);

    tft.fillCircle(centerX, centerY + 1, 3, TFT_RED);
}

void updateDigitalClock_analog(const char* timeStr) {
    // Șterge zona ceasului digital
    tft.fillRect(centerX - 50, centerY - 15, 100, 30, TFT_BLACK);

    // Desenează timpul digital
    tft.setTextDatum(MC_DATUM);  // Centrare text
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.drawString(timeStr, centerX, centerY + 80, 6);  // Font 4 // Coborât cu 20 de pixeli
}

void updateClockDisplay_analog() {
    // Sincronizează timpul NTP la fiecare 5 minute
    static unsigned long lastSyncTime = 0;
    const unsigned long syncInterval = 5 * 60 * 1000;
    unsigned long currentMillis = millis();
    
    // Sincronizează timpul NTP la fiecare 5 minute
    if (currentMillis - lastSyncTime >= syncInterval) {
        char timeStr[9];
        getNTPTime(timeStr, sizeof(timeStr));
        lastSyncTime = currentMillis;  // Actualizează timpul ultimei sincronizări
    }

    // Actualizează ceasul digital folosind timpul local
    struct tm timeinfo;
    if (getLocalTime(&timeinfo)) {
        char timeStr[9];
        snprintf(timeStr, sizeof(timeStr), "%02d:%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
        updateDigitalClock_analog(timeStr);  // Actualizează ceasul digital

        // Actualizează ceasul analog
        updateAnalogClock(timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);  // Actualizează ceasul analog
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




//////////////////////////////////////////////////////////////////////////////////////////////


WeatherData weatherData;  // Definirea obiectului weatherData

/// Funcția pentru recompunerea URL-ului
char* recomposeUrl_OW() {
    const char* baseUrl = "http://api.openweathermap.org/data/2.5/weather?";
    const char* lat = "lat=44.4323";
    const char* lon = "lon=26.1063";
    const char* appid = "appid=ccfc70a832d45188fad54e50731115d0";
    const char* units = "units=metric";

    static char weatherUrl[256]; // Static pentru a persista în afacerea funcției
    snprintf(weatherUrl, sizeof(weatherUrl), "%s%s&%s&%s&%s", baseUrl, lat, lon, appid, units);

    return weatherUrl;
}

// Funcția care preia datele meteo
void fetchWeatherData(WeatherData* weatherData) {
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        const char* weatherUrl = recomposeUrl_OW();  // Obținem URL-ul actualizat

        Serial.print("Requesting weather data from: ");
        Serial.println(weatherUrl);

        http.begin(weatherUrl);  // Folosim URL-ul obținut

        int httpResponseCode = http.GET();
        if (httpResponseCode > 0) {
            String payload = http.getString();
            Serial.println("HTTP Response:");
            Serial.println(payload);

            StaticJsonDocument<1024> doc;
            DeserializationError error = deserializeJson(doc, payload);

            if (!error) {
                weatherData->weatherId = doc["weather"][0]["id"];
                weatherData->mainTemp = doc["main"]["temp"];
                weatherData->windSpeed = doc["wind"]["speed"];
                weatherData->windDeg = doc["wind"]["deg"];
                weatherData->mainPressure = doc["main"]["pressure"];
                weatherData->mainHumidity = doc["main"]["humidity"];
                weatherData->sunrise = doc["sys"]["sunrise"];
                weatherData->sunset = doc["sys"]["sunset"];
                weatherData->dt = doc["dt"];
                weatherData->timezoneName = doc["timezone"];
            } else {
                Serial.print("JSON parsing failed: ");
                Serial.println(error.c_str());
            }
        } else {
            Serial.print("HTTP GET request failed with code: ");
            Serial.println(httpResponseCode);
        }
        http.end();
    } else {
        Serial.println("WiFi not connected");
    }
}

// Funcția pentru a afișa datele meteo
void displayWeatherData(const WeatherData* weatherData) {
    Serial.println("Weather Data:");

    Serial.print("Weather ID: ");
    Serial.println(weatherData->weatherId);

    Serial.print("Temperature: ");
    Serial.println(weatherData->mainTemp);

    Serial.print("Wind Speed: ");
    Serial.println(weatherData->windSpeed);

    Serial.print("Wind Degree: ");
    Serial.println(weatherData->windDeg);

    Serial.print("Pressure: ");
    Serial.println(weatherData->mainPressure);

    Serial.print("Humidity: ");
    Serial.println(weatherData->mainHumidity);

    Serial.print("Sunrise: ");
    Serial.println(weatherData->sunrise);

    Serial.print("Sunset: ");
    Serial.println(weatherData->sunset);

    Serial.print("Timestamp: ");
    Serial.println(weatherData->dt);

    Serial.print("Timezone: ");
    Serial.println(weatherData->timezoneName);
}


void temperature_image() {
    unsigned long currentMillis = millis();

    if (currentMillis - lastDrawTime >= drawInterval) {
        // Afișează imaginea doar la fiecare 2 secunde
        lastDrawTime = currentMillis;

        // Codul pentru a desena imaginea
        tft.setTextColor(0xFFFF, 0x0000);
        tft.fillScreen(TFT_BLACK);
        tft.setSwapBytes(true);

        TJpgDec.setJpgScale(2);
        TJpgDec.setCallback(tft_output);
        tft.fillScreen(TFT_BLACK);

        uint32_t t = millis();
        uint16_t w = 0, h = 0;
        TJpgDec.getFsJpgSize(&w, &h, "/Radio_jpg/temperature.jpg", LittleFS);
        //Serial.print("Width = ");
        //Serial.print(w);
        //Serial.print(", height = ");
        //Serial.println(h);

        uint16_t scaled_w = w / 2;
        uint16_t scaled_h = h / 2;
        int16_t x = (TFT_WIDTH - scaled_w+30) / 2;
        int16_t y = (TFT_HEIGHT - scaled_h-10) / 2;

        TJpgDec.drawFsJpg(x, y, "/Radio_jpg/temperature.jpg", LittleFS);

        t = millis() - t;
        //Serial.print(t);
        //Serial.println(" ms");
        Serial.println("Image drawing Temperature");
    }
}


void displayTemperature(const WeatherData* weatherData) {
    // Șterge ecranul înainte de afișare
    tft.fillScreen(TFT_BLACK);

    const int centerX = TFT_WIDTH / 2;  // Centrul orizontal
    const int centerY = TFT_HEIGHT / 2; // Centrul vertical

    // Afișează imaginea asociată temperaturii (dacă există)
    temperature_image();

    // Setează stilul textului
    tft.setTextDatum(MC_DATUM);      // Centrare text
    tft.setTextColor(TFT_DARKGREEN);   // Text portocaliu, fără fundal

    // Obține temperatura din structura WeatherData
    float temperature = weatherData->mainTemp;

    // Transformă temperatura într-un șir de caractere
    String tempString = String(temperature, 2);

    // Afișează temperatura pe ecran, centrată
    tft.drawString(tempString, centerX-43, centerY + 73, 6);  // Font 4

    // Afișează și alte detalii meteo opționale (exemplu: umiditate)
    //String humidityString = "Humidity: " + String(weatherData->mainHumidity) + "%";
    //tft.setTextColor(TFT_WHITE);  // Text alb
    //tft.drawString(humidityString, centerX, centerY + 120, 2);  // Font 2
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void displayNoiseLevel() {
    // Citește RSSI și SNR
    int rssi = si4735.getCurrentRSSI();
    int snr = si4735.getCurrentSNR();

    // Normalizează valorile pentru afișare și le crește cu 50% pentru a fi mai vizibile
    int rssiBarHeight = map(rssi, 0, 127, 0, 150) * 2.5;  // Înălțime bară RSSI crescută cu 50%
    int snrBarHeight = map(snr, 0, 127, 0, 150) * 2.5;      // Înălțime bară SNR crescută cu 50%

    // Asigură-te că înălțimea barelor nu depășește limita maximă de 150
    rssiBarHeight = min(rssiBarHeight, 150);  // Limitează la 150
    snrBarHeight = min(snrBarHeight, 150);    // Limitează la 150

    // Curăță ecranul
    tft.fillScreen(TFT_BLACK);

    // Afișează textul
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextDatum(TC_DATUM);  // Text centrat
    tft.drawString("Noise Spectrum", TFT_WIDTH / 2, 10, 2);  // Titlu

    // Afișează informațiile despre frecvență, RSSI și SNR
    char freqText[20];
    snprintf(freqText, sizeof(freqText), "F: %.2fMHz", si4735.getFrequency() / 100.0);
    tft.drawString(freqText, TFT_WIDTH / 2-100, 100, 2);  // Frecvență

    char rssiText[20];
    snprintf(rssiText, sizeof(rssiText), "R: %ddB", rssi);
    tft.drawString(rssiText, TFT_WIDTH / 2-100, 130, 2);  // RSSI

    char snrText[20];
    snprintf(snrText, sizeof(snrText), "S: %ddB", snr);
    tft.drawString(snrText, TFT_WIDTH / 2-100, 160, 2);  // SNR

    // Desenează barele
    tft.fillRect(60, TFT_HEIGHT - 160, 40, 150, TFT_DARKGREY); // Background RSSI
    tft.fillRect(140, TFT_HEIGHT - 160, 40, 150, TFT_DARKGREY); // Background SNR
    tft.fillRect(60, TFT_HEIGHT - 10 - rssiBarHeight, 40, rssiBarHeight, TFT_GREEN);  // Bară RSSI
    tft.fillRect(140, TFT_HEIGHT - 10 - snrBarHeight, 40, snrBarHeight, TFT_BLUE);   // Bară SNR

    // Afișează legendele
    tft.setTextDatum(TC_DATUM);
    tft.drawString("RSSI", 80, TFT_HEIGHT - 10, 1);  // Sub bara RSSI
    tft.drawString("SNR", 160, TFT_HEIGHT - 10, 1);  // Sub bara SNR
}














// in teste pentru moment//////////////////////////////////////////////////////////////////////////////

// Variabile pentru grafic
const int graphWidth = 200;   // Lățimea display-ului
const int graphHeight = 60;   // Înălțimea graficului
const int graphBaseY = 200;   // Linia de bază a graficului
int prevX = 0, prevY = graphBaseY; // Coordonatele anterioare ale graficului
int currentX = 0;

// Funcția pentru afișarea graficului RSSI
void grafic_rssi() {
    unsigned long startTime = millis(); // Timpul de început
    unsigned long duration = 30000;    // Durata de 30 de secunde

    tft.fillScreen(TFT_BLACK); // Curățare ecran
    tft.drawRect(20, graphBaseY - graphHeight - 30, graphWidth, graphHeight, TFT_WHITE); // Cadru grafic ridicat

    while (millis() - startTime < duration) {
        // Obține RSSI
        si4735.getCurrentReceivedSignalQuality(1); // Actualizare calitate semnal
        int rssi = si4735.getCurrentRSSI(); // Obține valoarea RSSI

        // Calculăm centrul dreptunghiului
        int centerY = graphBaseY - 30 - (graphHeight / 2);

        // Maparea valorii RSSI pe înălțimea graficului și centrare
        int mappedY = centerY - map(rssi, 0, 100, 0, graphHeight);

        // Desenare linie între punctele anterioare și curente
        tft.drawLine(prevX, prevY, currentX, mappedY, TFT_GREEN);

        // Actualizare coordonate
        prevX = currentX;
        prevY = mappedY;
        currentX = (currentX + 1) % graphWidth; // Resetare la capătul graficului

        // Ștergere linie veche pentru efect continuu (opțional)
        if (currentX == 0) {
            tft.fillRect(0, graphBaseY - graphHeight - 30, graphWidth, graphHeight, TFT_BLACK);
            tft.drawRect(20, graphBaseY - graphHeight - 30, graphWidth, graphHeight, TFT_WHITE);
        }

        vTaskDelay(100 / portTICK_PERIOD_MS); // Actualizare grafic la fiecare 100ms
    }
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////