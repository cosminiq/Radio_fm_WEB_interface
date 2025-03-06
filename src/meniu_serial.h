#ifndef MENIU_SERIAL_H
#define MENIU_SERIAL_H
#include <Wire.h>
#include <SI4735.h>
#include "serial_function.h"
#include "globals.h"
#include "posturi_fm_B.h"



extern SI4735 si4735;
extern uint16_t currentFrequency;
extern uint16_t previousFrequency;
extern bool muteState;

// Elimină #define și folosește enum pentru meniuri
enum MeniuNivel {
  MENIU_PRINCIPAL,
  SUBMENIU_SERIAL,
  SUBMENIU_FREQUENCY,
  SUBMENIU_FM_BUCURESTI
};

// Declară variabila nivelCurent
extern MeniuNivel nivelCurent;

void showHelp();
void showHelp_main_menu();
void showHelp_handleFrequencyInput();
void showHelp_handleSubmeniuPosturiFM();
void handleMeniuPrincipal(char input);
void handleSubmeniuSerial(char input);
void handleSubmeniuFrequency(char input);
void handleMeniuPrincipal(char input);
void handleSubmeniuPosturiFM(char input);


#endif