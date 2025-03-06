#ifndef POSTURI_FM_B_H
#define POSTURI_FM_B_H

#include <Arduino.h>  // Asigură-te că ai inclus această librărie pentru a folosi String
#include <SI4735.h>  // Dacă folosești un modul SI4735 pentru radio
#include "globals.h"

// Declară variabilele globale
extern uint16_t currentFrequency; // Variabila globală pentru frecvență
extern SI4735 si4735;  // Obiectul SI4735

// Structura pentru posturile FM
struct PostFM {
  String nume;
  float frecventa;
  String gen;
};

// Lista posturilor FM
extern PostFM posturiFM[];

// Declarația constantei numarPosturi
extern int numarPosturi;

void initPosturiFM();
void showPosturiFM();
void selecteazaPostFM(float frecventa);
bool selecteazaPostDupaInput(char input);

#endif
