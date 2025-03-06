#ifndef PLOTING_CPU_H
#define PLOTING_CPU_H

#include <TFT_eSPI.h>

// Definire constante
#define PLOT_WIDTH  220  // Lățimea zonei de plotare
#define PLOT_HEIGHT 120  // Înălțimea zonei de plotare
#define PLOT_X      20  // Poziția X a zonei de plotare
#define PLOT_Y      70   // Poziția Y a zonei de plotare
#define TEXT_X      65  // TEXT_X: Poziția pe axa X (orizontală) a colțului stânga-sus al sprite-ului pe ecran.
#define TEXT_Y      20  // TEXT_Y: Poziția pe axa Y (verticală) a colțului stânga-sus al sprite-ului pe ecran.


// Variabile globale
extern TFT_eSPI tft_2; // Obiect pentru display
extern TFT_eSprite graphSprite; // Sprite pentru grafic
extern TFT_eSprite textSprite; // Sprite pentru text



extern int cpuLoad; // Valoarea încărcăturii procesorului
extern int plotBuffer[PLOT_WIDTH]; // Buffer pentru stocarea valorilor
extern int plotIndex; // Indexul curent în buffer

// Prototipuri de funcții
int readCpuLoad();
void updatePlot();
void displayCpuLoadText(int cpuLoad);
void clearScreen(uint16_t color);

#endif