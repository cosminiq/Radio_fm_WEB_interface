#ifndef TIMER_NTP_H
#define TIMER_NTP_H

#include <Arduino.h>
#include <WiFi.h>
#include <time.h>
#include <ESPAsyncWebServer.h>
#include <SI4735.h>
#include "globals.h"
#include "TM1638_control.h"

// Funcții
void TM1638_displayNTPTime();


#endif // TIMER_NTP_H
