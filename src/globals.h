#ifndef GLOBALS_H
#define GLOBALS_H
#include <Wire.h>
#include <SI4735.h>
#include <TFT_eSPI.h>
#include <TimeLib.h>
#include "esp_system.h"
#include "RDS.h"
#include "meniu_serial.h"
#include "web_server_html.h" // Include fișierul care conține logica serverului web
#include "wifi_manager.h"
#include "Timer_ntp.h"
#include "TM1638_control.h"
#include "display_ST7789_round.h"




extern unsigned long lastRDSCheck;
extern const unsigned long RDS_CHECK_INTERVAL;

// Declarații externe pentru constante
extern const int AM_FUNCTION;
extern const int FM_FUNCTION;


// Pin definitions
extern const int RESET_PIN;
extern const int ESP32_I2C_SDA;
extern const int ESP32_I2C_SCL;

// Radio settings
extern const uint16_t AM_FREQUENCY_MIN;
extern const uint16_t AM_FREQUENCY_MAX;
extern const uint16_t FM_FREQUENCY_MIN;
extern const uint16_t FM_FREQUENCY_MAX;
extern const uint16_t DEFAULT_FM_FREQUENCY;

// Radio state variables
extern uint16_t currentFrequency;
extern uint16_t previousFrequency;
extern bool muteState;
extern uint8_t bandwidthIdx;
extern const char* bandwidth[];

// SI4735 object
extern SI4735 si4735;

#endif
