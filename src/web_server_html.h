#ifndef WEB_SERVER_HTML_H
#define WEB_SERVER_HTML_H
#define FISIER_CSV "/wifi/pass.csv"  // Calea către fișierul CSV în memoria internă
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
#include <LittleFS.h>
#include <FS.h>
#include <SI4735.h>
#include <ArduinoJson.h>
#include "globals.h"




// Declarații funcții
void Server_ESP_32_Control_FM_WEB();
void serveStaticFiles();


extern AsyncWebServer server;

#endif
