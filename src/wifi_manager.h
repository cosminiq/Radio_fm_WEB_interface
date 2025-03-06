#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <WiFi.h>
#include <LittleFS.h>

// Calea fișierului CSV pentru credentiale Wi-Fi
#define WIFI_CREDENTIALS_FILE "/wifi/pass.csv"

// Declarații funcții
bool connectToWiFi(const char* ssid, const char* password);
void saveWiFiCredentials(const char* ssid, const char* password);
void scanAndConnectWiFi();
void startHotspot();

#endif
