#include "globals.h"




// vARIABILE GLOBALE lopp ca si task pentru functia RDS
// Inițializarea variabilelor globale
unsigned long lastRDSCheck = 0;
const unsigned long RDS_CHECK_INTERVAL = 10000; // 10 secunde


// Definițiile constantei
const int AM_FUNCTION = 1;
const int FM_FUNCTION = 0;


// Pin definitions
const int RESET_PIN = 12;
const int ESP32_I2C_SDA = 25;  //21;     // GPIO21
const int ESP32_I2C_SCL = 33;  //22;     // GPIO22

// Radio settings
const uint16_t AM_FREQUENCY_MIN = 570;
const uint16_t AM_FREQUENCY_MAX = 1710;
const uint16_t FM_FREQUENCY_MIN = 8600;
const uint16_t FM_FREQUENCY_MAX = 10800;
const uint16_t DEFAULT_FM_FREQUENCY = 9080;

// Radio state variables
uint16_t currentFrequency = 0;
uint16_t previousFrequency = 0;
bool muteState = false;
uint8_t bandwidthIdx = 0;
const char* bandwidth[] = {"6", "4", "3", "2", "1", "1.8", "2.5"};

// SI4735 object
SI4735 si4735;
