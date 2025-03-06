#ifndef TM1638_CONTROL_H
#define TM1638_CONTROL_H

#include <TM1638plus.h> // Include the library
#include <LittleFS.h>
#include "serial_function.h"
#include "globals.h"
#include <Wire.h>
#include <SI4735.h>
#include <vector>
#include <TFT_eSPI.h>
#include "ploting_cpu.h"

// GPIO I/O pins on the Arduino connected to strobe, clock, data
#define  STROBE_TM 14 // strobe = GPIO connected to strobe line of module
#define  CLOCK_TM 27  // clock = GPIO connected to clock line of module
#define  DIO_TM 26 // data = GPIO connected to data line of module
#define TFT_BACKLIGHT_ON HIGH  // Nivelul pentru aprinderea backlight-ului (HIGH sau LOW)

extern bool high_freq; // Frequency setting for high-frequency CPUs
extern const long intervalButton; // Interval to read button (milliseconds)
extern const long intervalDisplay; // Interval at which to change display (milliseconds)

// Constructor object (GPIO STB, GPIO CLOCK, GPIO DIO, use high freq MCU)
extern TM1638plus tm;

// Function declarations
void setupTM1638();
void TM1638_mute();

void TM1638_handleVolumeUp();
void TM1638_handleVolumeDown();

void TM1638_handleFrequencyUp();
void TM1638_handleFrequencyDown();

void loadRadioStations();
void displayCurrentFrequency();

void TM1638_CSV_handleFrequencyUp();
void TM1638_CSV_handleFrequencyDown();



void TM1638_displayWaitingDotsAndLEDs();



void TM1638_handleTestButton();

#endif // TM1638_CONTROL_H
