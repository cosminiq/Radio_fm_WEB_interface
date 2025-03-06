


#include "ploting_cpu.h"

// Definire variabile globale
TFT_eSPI tft_2; // Obiect pentru display
TFT_eSprite graphSprite = TFT_eSprite(&tft_2); // Sprite pentru grafic
TFT_eSprite textSprite = TFT_eSprite(&tft_2);  // Sprite pentru text
TFT_eSprite backgroundSprite = TFT_eSprite(&tft_2); // Sprite pentru fundal

int plotBuffer[PLOT_WIDTH]; // Buffer pentru stocarea valorilor
int plotIndex = 0; // Indexul curent în buffer
int cpuLoad = readCpuLoad();
int minLoad = 100; // Valoarea minimă a încărcării procesorului
int maxLoad = 0;   // Valoarea maximă a încărcării procesorului

// Funcție pentru citirea încărcăturii procesorului de pe serial
int readCpuLoad() {
    if (Serial.available()) {
        String data = Serial.readStringUntil('\n'); // Citește până la newline
        data.trim(); // Elimină spațiile și newline-urile

        // Verifică dacă datele încep cu "CPU:"
        if (data.startsWith("CPU:")) {
            // Extrage valoarea după "CPU:"
            String valueStr = data.substring(4); // Ignoră primele 4 caractere ("CPU:")
            return valueStr.toInt(); // Converteste în întreg
        }
    }
    return -1; // Returnează -1 dacă nu sunt date utile
}

// Desenează axele și etichetele în sprite
void drawAxes() {
    // Desenează axa X (linia de bază)
    graphSprite.drawLine(0, PLOT_HEIGHT, PLOT_WIDTH, PLOT_HEIGHT, TFT_WHITE);

    // Desenează etichetele axei Y
    graphSprite.setTextColor(TFT_ORANGE, TFT_BLACK);
    graphSprite.setTextSize(1);
    for (int y = 0; y <= PLOT_HEIGHT; y += 20) {
        int value = map(y, PLOT_HEIGHT, 0, minLoad, maxLoad); // Valoarea corespunzătoare pe axa Y
        graphSprite.setCursor(0, y - 5); // Poziționează eticheta
        graphSprite.print(value); // Desenează valoarea
    }
}

// Desenează grila în sprite
void drawGrid() {
    graphSprite.setTextColor(TFT_DARKGREY, TFT_BLACK);
    for (int y = 0; y <= PLOT_HEIGHT; y += 10) {
        graphSprite.drawLine(0, y, PLOT_WIDTH, y, TFT_DARKGREY);
    }
/* 
    for (int x = 0; x <= PLOT_WIDTH; x += 5) {
        graphSprite.drawLine(x, 0, x, PLOT_HEIGHT, TFT_DARKGREY);
    }
 */
}

// Actualizează și desenează graficul
void updatePlot() {
    int cpuLoad = readCpuLoad();

    int barWidth = 10; // Lățimea unei bare
    
    if (cpuLoad != -1) {
        displayCpuLoadText(cpuLoad);

        if (cpuLoad < minLoad) minLoad = cpuLoad;
        if (cpuLoad > maxLoad) maxLoad = cpuLoad;
        if (minLoad == maxLoad) maxLoad = minLoad + 1; 

        // Mapează valoarea
        int newBarHeight = map(cpuLoad, minLoad, maxLoad, 0, PLOT_HEIGHT);
        plotBuffer[plotIndex] = newBarHeight;
        plotIndex = (plotIndex + 1) % (PLOT_WIDTH / barWidth); 

        // 1. Scroll hardware la nivel de bară
        graphSprite.scroll(-barWidth, 0);

        // 2. Redesenăm grila pentru a rămâne fixă
        drawGrid();  // Cheamă funcția de desenare a grilei

        // 3. Desenăm axele după grid
        drawAxes(); // Asigură-te că axele sunt redesenate după grid

        // 4. Desenează noua bară la capătul din dreapta
        uint16_t barColor = (cpuLoad < 30) ? TFT_GREEN : (cpuLoad < 70) ? TFT_ORANGE : TFT_RED;
        graphSprite.fillRect(PLOT_WIDTH - barWidth, PLOT_HEIGHT - newBarHeight, barWidth, newBarHeight, barColor);

        // 5. Transferă sprite-ul pe ecran
        graphSprite.pushSprite(PLOT_X, PLOT_Y);
    }
}

// Funcție pentru afișarea încărcăturii procesorului sub formă de text
void displayCpuLoadText(int cpuLoad) {
    // Șterge conținutul anterior al sprite-ului pentru text
    textSprite.fillSprite(TFT_BLACK); // Fundal negru pentru text

    // Setează culoarea și alinierea textului
    textSprite.setTextColor(TFT_WHITE, TFT_BLACK); // Text alb pe fundal negru
    textSprite.setTextDatum(TC_DATUM); // Text centrat

    // Construiește textul folosind valoarea primită prin serial
    String text = "CPU: " + String(cpuLoad) + "%";

    // Desenează textul în sprite-ul pentru text
    textSprite.drawString(text, textSprite.width() / 2, 10, 4); // Text centrat pe lățimea sprite-ului

    // Transferă sprite-ul pentru text pe ecran
    textSprite.pushSprite(TEXT_X, TEXT_Y); // Poziționarea textului in sprite-ul textSprite
}

// Funcție pentru ștergerea ecranului cu o culoare specificată
void clearScreen(uint16_t color) {
    // Umple sprite-ul mare cu culoarea dorită
    backgroundSprite.fillSprite(color);

    // Afișează sprite-ul pe întregul ecran
    backgroundSprite.pushSprite(0, 0);
}



















/* 
// Functie pentru desenarea grid-ului
void drawGrid() {
    int gridSpacingX = 10; // Distanța între liniile verticale
    int gridSpacingY = 10; // Distanța între liniile orizontale
    for (int x = 0; x <= PLOT_WIDTH; x += gridSpacingX) {
        graphSprite.drawLine(x, 0, x, PLOT_HEIGHT, TFT_DARKGREY); // Linii verticale
    }
    for (int y = 0; y <= PLOT_HEIGHT; y += gridSpacingY) {
        graphSprite.drawLine(0, y, PLOT_WIDTH, y, TFT_DARKGREY); // Linii orizontale
    }
}

 */

/* void drawGrid() {
    graphSprite.setTextColor(TFT_DARKGREY, TFT_BLACK);
    for (int y = 0; y <= PLOT_HEIGHT; y += 5) {
        graphSprite.drawLine(0, y, PLOT_WIDTH, y, TFT_DARKGREY);
    }
    for (int x = 0; x <= PLOT_WIDTH; x += 5) {
        graphSprite.drawLine(x, 0, x, PLOT_HEIGHT, TFT_DARKGREY);
    }
}
 */



/* 
// Desenează barele colorate în sprite
void drawBars() {
    int barWidth = 5; // Lățimea unei bare
    int baselineY = PLOT_HEIGHT; // Poziția axei X (josul sprite-ului)
    for (int i = 0; i < PLOT_WIDTH; i += barWidth) {
        int x = i;
        int y = plotBuffer[(plotIndex + i) % PLOT_WIDTH]; // Înălțimea barei

        // Alege culoarea în funcție de valoarea barei
        uint16_t barColor;
        int value = map(y, 0, PLOT_HEIGHT, 0, 100); // Valoarea barei
        if (value < 50) {
            barColor = TFT_GREEN; // Verde pentru valori mici
        } else if (value < 80) {
            barColor = TFT_YELLOW; // Galben pentru valori medii
        } else {
            barColor = TFT_RED; // Roșu pentru valori mari
        }

        // Desenează bara
        graphSprite.fillRect(x, baselineY - y, barWidth, y, barColor);
    }
}

 */




/* 
void updatePlot() {
    // Șterge sprite-ul înainte de redesenare


    // Desenează grila
    drawGrid();

    // Desenează axele și etichetele
    drawAxes();

    int cpuLoad = readCpuLoad();
    int barWidth = 5; // Lățimea unei bare
    
    if (cpuLoad != -1) {
        displayCpuLoadText(cpuLoad);

        if (cpuLoad < minLoad) minLoad = cpuLoad;
        if (cpuLoad > maxLoad) maxLoad = cpuLoad;
        if (minLoad == maxLoad) maxLoad = minLoad + 1; 

        // Adaugă noua valoare în buffer
        int newBarHeight = map(cpuLoad, minLoad, maxLoad, 0, PLOT_HEIGHT);
        plotBuffer[plotIndex] = newBarHeight;
        plotIndex = (plotIndex + 1) % (PLOT_WIDTH / barWidth); 

        // Scroll hardware la nivel de bară (nu pixel cu pixel!)
        graphSprite.scroll(-barWidth, 0); 
        
        // Șterge zona nouă unde se va adăuga bara nouă
        graphSprite.fillRect(PLOT_WIDTH - barWidth, 0, barWidth, PLOT_HEIGHT, TFT_BLACK);
        
        // Desenează noua bară la capătul din dreapta
        uint16_t barColor = (cpuLoad < 50) ? TFT_GREEN : (cpuLoad < 80) ? TFT_YELLOW : TFT_RED;
        graphSprite.fillRect(PLOT_WIDTH - barWidth, PLOT_HEIGHT - newBarHeight, barWidth, newBarHeight, barColor);

        
        graphSprite.pushSprite(PLOT_X, PLOT_Y);
       
    

    }
}

 */







/* // Funcția principală pentru actualizarea graficului
void updatePlot() {
    // Citește încărcarea procesorului de pe serial
    int cpuLoad = readCpuLoad();

    // Dacă s-au primit date valide
    if (cpuLoad != -1) {
        // Afișează textul CPU load
        displayCpuLoadText(cpuLoad);

        // Actualizează valorile minime și maxime
        if (cpuLoad < minLoad) minLoad = cpuLoad;
        if (cpuLoad > maxLoad) maxLoad = cpuLoad;

        if (minLoad == maxLoad) {
            maxLoad = minLoad + 1; // Evită împărțirea la zero
        }

        // Adaugă noua valoare în buffer-ul de plotare
        plotBuffer[plotIndex] = map(cpuLoad, minLoad, maxLoad, 0, PLOT_HEIGHT);
        plotIndex = (plotIndex + 1) % PLOT_WIDTH;

        // Desenează graficul în sprite
        graphSprite.fillSprite(TFT_BLACK); // Șterge conținutul anterior
        drawGrid(); // Desenează grid-ul
        drawAxes(); // Desenează axele și etichetele
        drawBars(); // Desenează barele colorate

        // Transferă sprite-ul pe ecran
        graphSprite.pushSprite(PLOT_X, PLOT_Y); // Desenează sprite-ul în zona de plotare
    }
}


 */



/* 
// Funcție pentru ștergerea ecranului cu o culoare specificată


 */



// Funcție pentru actualizarea și desenarea graficului
/*
// Funcție pentru actualizarea și desenarea graficului
void updatePlot() {
    // Citește încărcarea procesorului de pe serial
    int cpuLoad = readCpuLoad();

    // Dacă s-au primit date valide
    if (cpuLoad != -1) {
        displayCpuLoadText(cpuLoad);
        // Actualizează valorile minime și maxime
        if (cpuLoad < minLoad) minLoad = cpuLoad;
        if (cpuLoad > maxLoad) maxLoad = cpuLoad;


        if (minLoad == maxLoad) {
        // Dacă minLoad și maxLoad sunt egale, ajustează maxLoad
        maxLoad = minLoad + 1; // Adaugă 1 pentru a evita împărțirea la zero
        }

        // Acum poți folosi map() în siguranță
        plotBuffer[plotIndex] = map(cpuLoad, minLoad, maxLoad, PLOT_HEIGHT, 0);
        // Adaugă noua valoare în buffer
        plotBuffer[plotIndex] = map(cpuLoad, minLoad, maxLoad, PLOT_HEIGHT, 0); // Mapează valoarea la înălțimea zonei de plotare
        plotIndex = (plotIndex + 1) % PLOT_WIDTH; // Actualizează indexul

        // Desenează graficul în sprite
        graphSprite.fillSprite(TFT_BLACK); // Șterge conținutul anterior al sprite-ului

        // Desenează grid-ul
        int gridSpacingX = 10; // Distanța între liniile verticale
        int gridSpacingY = 10; // Distanța între liniile orizontale
        for (int x = 0; x <= PLOT_WIDTH; x += gridSpacingX) {
            graphSprite.drawLine(x, 0, x, PLOT_HEIGHT, TFT_DARKGREY); // Linii verticale
        }
        for (int y = 0; y <= PLOT_HEIGHT; y += gridSpacingY) {
            graphSprite.drawLine(0, y, PLOT_WIDTH, y, TFT_DARKGREY); // Linii orizontale
        }

        // Etichete pentru axa Y
        for (int y = 0; y <= PLOT_HEIGHT; y += gridSpacingY * 2) {
            int value = map(y, PLOT_HEIGHT, 0, minLoad, maxLoad); // Valoarea corespunzătoare pe axa Y
            graphSprite.setTextColor(TFT_WHITE, TFT_BLACK);
            graphSprite.setCursor(0, y - 5);
            graphSprite.setTextSize(1); // Dimensiunea textului (1 este implicit, 2 este dublu, etc.)
            graphSprite.setTextFont(1); // Fontul implicit GLCD
            graphSprite.print(value);
        }

        // Desenează axa x (linia de bază)
        int baselineY = PLOT_HEIGHT; // Poziția axei x (josul sprite-ului)
        graphSprite.drawLine(0, baselineY, PLOT_WIDTH, baselineY, TFT_MAGENTA); // Linie magenta pentru axa x

        // Desenează graficul ca o linie frântă și umple zona de sub el
        int step = 5; // Pasul între puncte (ajustează pentru distanța dorită între linii)
        for (int i = 0; i < PLOT_WIDTH - step; i += step) {
            int x0 = i;
            int y0 = plotBuffer[(plotIndex + i) % PLOT_WIDTH];
            int x1 = i + step;
            int y1 = plotBuffer[(plotIndex + i + step) % PLOT_WIDTH];

            // Desenează linia frântă (conectează punctele consecutive)
            graphSprite.drawLine(x0, y0, x1, y1, TFT_RED);

            // Umple zona de sub grafic
            graphSprite.fillTriangle(
                x0, y0, // Vârful 1 al triunghiului
                x1, y1, // Vârful 2 al triunghiului
                x1, baselineY, // Vârful 3 al triunghiului (pe axa x)
                TFT_GREEN // Culoarea de umplere
            );
            graphSprite.fillTriangle(
                x0, y0, // Vârful 1 al triunghiului
                x0, baselineY, // Vârful 2 al triunghiului (pe axa x)
                x1, baselineY, // Vârful 3 al triunghiului (pe axa x)
                TFT_GREEN // Culoarea de umplere
            );
        }

        // Transferă sprite-ul pe display
        graphSprite.pushSprite(PLOT_X, PLOT_Y); // Desenează sprite-ul în zona de plotare
    }
}

*/

//PLOTER CPU CU TRIUNGHIRI GALBENE
/*  
#include "ploting_cpu.h"

// Definire variabile globale
TFT_eSPI tft_2; // Obiect pentru display
TFT_eSprite graphSprite = TFT_eSprite(&tft_2); // Sprite pentru grafic

int plotBuffer[PLOT_WIDTH]; // Buffer pentru stocarea valorilor
int plotIndex = 0; // Indexul curent în buffer
int cpuLoad = readCpuLoad();
// Funcție pentru citirea încărcăturii procesorului de pe serial
int readCpuLoad() {
    if (Serial.available()) {
        String data = Serial.readStringUntil('\n'); // Citește până la newline
        data.trim(); // Elimină spațiile și newline-urile

        // Verifică dacă datele încep cu "CPU:"
        if (data.startsWith("CPU:")) {
            // Extrage valoarea după "CPU:"
            String valueStr = data.substring(4); // Ignoră primele 4 caractere ("CPU:")
            return valueStr.toInt(); // Converteste în întreg
        }
    }
    return -1; // Returnează -1 dacă nu sunt date utile
}

void updatePlot() {
    // Citește încărcarea procesorului de pe serial
    int cpuLoad = readCpuLoad();

    // Dacă s-au primit date valide
    if (cpuLoad != -1) {
        // Adaugă noua valoare în buffer
        plotBuffer[plotIndex] = map(cpuLoad, 0, 100, PLOT_HEIGHT, 0); // Mapează valoarea la înălțimea zonei de plotare
        plotIndex = (plotIndex + 1) % PLOT_WIDTH; // Actualizează indexul

        // Desenează graficul în sprite
        graphSprite.fillSprite(TFT_BLACK); // Șterge conținutul anterior al sprite-ului

        // Desenează grid-ul
        int gridSpacingX = 10; // Distanța între liniile verticale
        int gridSpacingY = 10; // Distanța între liniile orizontale
        for (int x = 0; x <= PLOT_WIDTH; x += gridSpacingX) {
            graphSprite.drawLine(x, 0, x, PLOT_HEIGHT, TFT_DARKGREY); // Linii verticale
        }
        for (int y = 0; y <= PLOT_HEIGHT; y += gridSpacingY) {
            graphSprite.drawLine(0, y, PLOT_WIDTH, y, TFT_DARKGREY); // Linii orizontale
        }

        // Etichete pentru axa Y
        for (int y = 0; y <= PLOT_HEIGHT; y += gridSpacingY * 2) {
            int value = map(y, PLOT_HEIGHT, 0, 0, 100); // Valoarea corespunzătoare pe axa Y
            graphSprite.setTextColor(TFT_WHITE, TFT_BLACK);
            graphSprite.setCursor(0, y - 5);
            graphSprite.setTextSize(1); // Dimensiunea textului (1 este implicit, 2 este dublu, etc.)
            graphSprite.setTextFont(1); // Fontul implicit GLCD
            graphSprite.print(value);
        }

        // Desenează axa x (linia de bază)
        int baselineY = PLOT_HEIGHT; // Poziția axei x (josul sprite-ului)
        graphSprite.drawLine(0, baselineY, PLOT_WIDTH, baselineY, TFT_MAGENTA); // Linie magenta pentru axa x

        // Desenează graficul ca o linie frântă și umple zona de sub el
        int step = 5; // Pasul între puncte (ajustează pentru distanța dorită între linii)
        for (int i = 0; i < PLOT_WIDTH - step; i += step) {
            int x0 = i;
            int y0 = plotBuffer[(plotIndex + i) % PLOT_WIDTH];
            int x1 = i + step;
            int y1 = plotBuffer[(plotIndex + i + step) % PLOT_WIDTH];

            // Desenează linia frântă (conectează punctele consecutive)
            graphSprite.drawLine(x0, y0, x1, y1, TFT_RED);

            // Umple zona de sub grafic
            graphSprite.fillTriangle(
                x0, y0, // Vârful 1 al triunghiului
                x1, y1, // Vârful 2 al triunghiului
                x1, baselineY, // Vârful 3 al triunghiului (pe axa x)
                TFT_ORANGE // Culoarea de umplere
            );
            graphSprite.fillTriangle(
                x0, y0, // Vârful 1 al triunghiului
                x0, baselineY, // Vârful 2 al triunghiului (pe axa x)
                x1, baselineY, // Vârful 3 al triunghiului (pe axa x)
                TFT_ORANGE // Culoarea de umplere
            );


    

        
        }

        // Transferă sprite-ul pe display
        graphSprite.pushSprite(PLOT_X, PLOT_Y); // Desenează sprite-ul în zona de plotare

    }
}




void displayCpuLoadText(int cpuLoad) {
    int cpuLoad_2 = readCpuLoad();


    // Setează culoarea și alinierea textului
    graphSprite.setTextColor(TFT_WHITE);
    graphSprite.setTextDatum(TC_DATUM); // Text centrat

    // Construiește textul
    String text = "CPU load: " + String(cpuLoad) + "%";

    // Desenează textul în sprite
    graphSprite.drawString(text, graphSprite.width() / 2, 0, 2); // Text centrat pe lățimea sprite-ului

    // Transferă sprite-ul pe ecran
    graphSprite.pushSprite(TEXT_X, TEXT_Y); // Desenează sprite-ul deasupra graficului
}

*/