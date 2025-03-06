// display_ST7789_round.h
#ifndef DISPLAY_ST7789_ROUND_H
#define DISPLAY_ST7789_ROUND_H

#include <TFT_eSPI.h>
#include <LittleFS.h> // Include the LittleFS library
#include <TJpg_Decoder.h>
#include <WiFi.h>
#include <SI4735.h>
#include <time.h>


#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <cstdio> // Adăugat pentru snprintf

struct WeatherData {
    uint16_t weatherId;
    float mainTemp;
    float windSpeed;
    float windDeg;
    int mainPressure;
    int mainHumidity;
    unsigned long sunrise;
    unsigned long sunset;
    unsigned long dt;
    const char* timezoneName;
};

extern WeatherData weatherData;
extern TFT_eSPI tft;
extern SI4735 si4735;

void initDisplay(TFT_eSPI &tft); // Inițializează display-ul
void updateAnalogClock(TFT_eSPI &tft, uint8_t hh, uint8_t mm, uint8_t ss); // Actualizează ceasul analog
void updateDigitalClock(TFT_eSPI &tft, const char* timeStr);  //  Actualizează ceasul digital
void initNTP(); // Funcție pentru inițializarea NTP
void getNTPTime(char* timeStr, size_t len); // Funcție pentru a obține timpul NTP
void updateClockDisplay_analog();


void drawImage(); // Funcție pentru desenarea imaginii
void radio_image(); // Funcție pentru afișarea imaginii radio
void clock_image(); // Funcție pentru afișarea imaginii ceasului
void wifi_image(); // Funcție pentru afișarea imaginii wifi
void initDisplay_analog();



// Funcții pentru task-uri
void scrollTransition(void (*newDisplayFunction)()); // Funcție pentru tranziția între afișaje
void updateClockDisplay(); // Actualizează afișajul ceasului
void displayWiFiIP(); // Funcție pentru afișarea adresei IP
void displayCurrentFrequency_ST7789(); // Funcție pentru afișarea frecvenței curente
void fetchWeatherData(WeatherData* weatherData); // Funcție pentru a prelua datele meteo
void displayTemperature(const WeatherData* weatherData); // Funcție pentru a afișa temperatura
void displayWeatherData(const WeatherData* weatherData);    // Funcție pentru a afișa datele meteo
void temperature_image(); // Funcție pentru afișarea imaginii temperaturii
void displayNoiseLevel(); // Funcție pentru afișarea nivelului de zgomot



void grafic_rssi(); // Funcție pentru afișarea graficului RSSI
#endif // DISPLAY_ST7789_ROUND_H
