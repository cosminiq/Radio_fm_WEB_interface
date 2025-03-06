#include "TM1638_control.h"

// Include the header file with the function declarations
extern bool isPlotUpdateRunning; // Declarație externă pentru flag
extern bool isSerialMonitorRunning; // Declarație externă pentru flag
extern SemaphoreHandle_t meniuSemaphore; // Declarație externă pentru semafor
extern SemaphoreHandle_t mutex; // Declarație externă pentru mutex


// Define global variables
bool high_freq = true; // default false, If using a high freq CPU > ~100 MHZ set to true
const long intervalButton = 200; // interval to read button (milliseconds)
const long intervalDisplay = 1000; // interval at which to change display (milliseconds)

// Constructor object (GPIO STB , GPIO CLOCK , GPIO DIO, use high freq MCU)
TM1638plus tm(STROBE_TM, CLOCK_TM, DIO_TM, high_freq);

// Variabile globale
bool isMuted = false; // Starea mute/unmute

void setupTM1638() {
    tm.displayBegin();
    tm.displayText("WELCOME");
}


void TM1638_mute() {
    static unsigned long buttonPressStartTime = 0; // Momentul când butonul este apăsat
    static bool isLongPressDetected = false;       // Indicator pentru apăsare lungă detectată
    uint8_t buttonsValue = tm.readButtons();

    // Verificăm dacă butonul 8 este apăsat
    if (buttonsValue & 0x01) { // 0x01 = bitul 1 (butonul 1)
        if (buttonPressStartTime == 0) {
            // Înregistrăm momentul când butonul este apăsat
            buttonPressStartTime = millis();
            isLongPressDetected = false;
        } else if (millis() - buttonPressStartTime > 3000 && !isLongPressDetected) {
            // Dacă butonul este apăsat mai mult de 3 secunde și nu a fost deja detectată o apăsare lungă
            isLongPressDetected = true; // Marcăm că este o apăsare lungă

            // Activează sau dezactivează task-ul de actualizare a plotului
            if (xSemaphoreTake(mutex, portMAX_DELAY) == pdTRUE) {
                isPlotUpdateRunning = !isPlotUpdateRunning; // Comută starea
                isSerialMonitorRunning = !isPlotUpdateRunning; // Meniul rulează doar când plotter-ul este oprit
                xSemaphoreGive(mutex);
            }

            if (isPlotUpdateRunning) {
                tft_2.fillScreen(TFT_BLACK); // Șterge întregul display
                clearScreen(TFT_BLACK); // Șterge întregul display de sprite-uri
                Serial.println("Start_CPU_monitor");
            } else {
                Serial.println("Stop_CPU_monitor");
                showHelp_main_menu(); // Afișează meniul principal
            }
        }
    } else {
        // Dacă butonul nu mai este apăsat
        if (buttonPressStartTime > 0 && !isLongPressDetected) {
            // Funcționalitatea normală pentru apăsare scurtă (mute/unmute)
            isMuted = !isMuted; // Schimbă starea mute/unmute

            // Actualizare stare radio
            if (isMuted) {
                si4735.setGpio(0, 0, 0); // Mute radio
                tm.displayText("    MUTE");
                Serial.println("Radio Muted");
            } else {
                si4735.setGpio(1, 0, 0); // Unmute radio
                tm.displayText("  UNMUTE");
                Serial.println("Radio Unmuted");
            }
        }

        // Resetăm temporizatorul și starea de apăsare lungă
        buttonPressStartTime = 0;
        isLongPressDetected = false;
    }

    // Debouncing simplu
    delay(100);
}











/*
void TM1638_mute() {
    static unsigned long buttonPressStartTime = 0; // Momentul când butonul este apăsat
    static bool isLongPressDetected = false;       // Indicator pentru apăsare lungă detectată
    uint8_t buttonsValue = tm.readButtons();

    // Verificăm dacă butonul 8 este apăsat
    if (buttonsValue & 0x01) { // 0x01 = bitul 1 (butonul 1)
        if (buttonPressStartTime == 0) {
            // Înregistrăm momentul când butonul este apăsat
            buttonPressStartTime = millis();
            isLongPressDetected = false;
        } else if (millis() - buttonPressStartTime > 3000 && !isLongPressDetected) {
            // Dacă butonul este apăsat mai mult de 3 secunde și nu a fost deja detectată o apăsare lungă
            isLongPressDetected = true; // Marcăm că este o apăsare lungă

            // Activează sau dezactivează task-ul de actualizare a plotului
            isPlotUpdateRunning = !isPlotUpdateRunning; // Comută starea

            if (isPlotUpdateRunning) {
                isSerialMonitorRunning = false; // Plotter-ul rulează, deci meniul rămâne blocat
                tft_2.fillScreen(TFT_BLACK); // Șterge întregul display
                Serial.println("Start_CPU_monitor");
                //xSemaphoreGive(meniuSemaphore); // Eliberează semaforul pentru a permite task-ului Meniu să ruleze
            } else {
                Serial.println("Stop_CPU_monitor");
                isSerialMonitorRunning = true; // Plotter-ul este oprit, meniul poate fi deblocat
                // Eliberează semaforul pentru a permite task-ului Meniu să ruleze
                //xSemaphoreGive(meniuSemaphore); 
            }
        }
    } else {
        // Dacă butonul nu mai este apăsat
        if (buttonPressStartTime > 0 && !isLongPressDetected) {
            // Funcționalitatea normală pentru apăsare scurtă (mute/unmute)
            isMuted = !isMuted; // Schimbă starea mute/unmute

            // Actualizare stare radio
            if (isMuted) {
                si4735.setGpio(0, 0, 0); // Mute radio
                tm.displayText("    MUTE");
                Serial.println("Radio Muted");
            } else {
                si4735.setGpio(1, 0, 0); // Unmute radio
                tm.displayText("  UNMUTE");
                Serial.println("Radio Unmuted");
            }
        }

        // Resetăm temporizatorul și starea de apăsare lungă
        buttonPressStartTime = 0;
        isLongPressDetected = false;
    }

    // Debouncing simplu
    delay(100);
}
*/




/*
void TM1638_mute() {
    uint8_t buttonsValue = tm.readButtons();

    // Verificăm dacă butonul 8 este apăsat
    if (buttonsValue & 0x01) { // 0x80 = bitul 8
        isMuted = !isMuted; // Schimbă starea mute/unmute

        // Actualizare stare radio
        if (isMuted) {
            si4735.setGpio(0, 0, 0); // Mute radio
            tm.displayText("    MUTE");
            Serial.println("Radio Muted");
        } else {
            si4735.setGpio(1, 0, 0); // Unmute radio
            tm.displayText("  UNMUTE");
            Serial.println("Radio Unmuted");
        }

        // Debouncing simplu
        delay(300);
    }
}
*/


int M1638__currentVolume = 10; // Volum inițial (scalează între 0-15 pentru SI4735)

void TM1638_handleVolumeUp() {
    uint8_t buttonsValue = tm.readButtons();

    // Verificăm dacă butonul 6 este apăsat (Volume Up)
    if (buttonsValue & 0x02) { // 0x02 = bitul 2
        si4735.volumeUp(); // Crește volumul
        M1638__currentVolume++; // Incrementăm variabila volumului

        // Creăm textul pentru afișaj, cu padding pentru a acoperi complet display-ul
        char displayText[9]; // TM1638 are un display de 8 caractere + terminator
        snprintf(displayText, sizeof(displayText), "VOL   %2d   ", M1638__currentVolume); // Formatare cu spații

        tm.displayText(displayText); // Afișăm textul formatat
        Serial.print("Volume Up: ");
        Serial.println(M1638__currentVolume);

        // Debouncing simplu
        delay(300);
    }
}


void TM1638_handleVolumeDown() {
    uint8_t buttonsValue = tm.readButtons();

    // Verificăm dacă butonul 7 este apăsat (Volume Down)
    if (buttonsValue & 0x04) { // 0x04 = bitul 3
        si4735.volumeDown(); // Scade volumul
        M1638__currentVolume--; // Decrementăm variabila volumului

        // Creăm textul pentru afișaj, cu padding pentru a acoperi complet display-ul
        char displayText[9]; // TM1638 are un display de 8 caractere + terminator
        snprintf(displayText, sizeof(displayText), "VOL   %2d   ", M1638__currentVolume); // Formatare cu spații
        
        tm.displayText(displayText); // Afișăm textul formatat
        Serial.print("Volume Down: ");
        Serial.println(M1638__currentVolume);

        // Debouncing simplu
        delay(300);
    }
}




void TM1638_handleFrequencyUp() {
    uint8_t buttonsValue = tm.readButtons();

    // Check if button 4 is pressed (Frequency Up)
    if (buttonsValue & 0x08) { // 0x10 = bit for button 4
        si4735.frequencyUp();  // Increase the frequency


        // Retrieve the current frequency and convert to MHz
        uint16_t currentFrequency = si4735.getFrequency(); // Assuming getFrequency() exists
        float currentFrequencyMHz = currentFrequency / 100.0; // Convert kHz to MHz

        tm.displayText("        ");  // Ștergem textul precedent
        // Display the frequency on the TM1638 display
        String displayText = "FRE " + String(currentFrequencyMHz, 1); // Display with 1 decimal places
        tm.displayText(displayText.c_str());  // Convert to const char* and display

        // Print to serial
        Serial.print("Current Frequency: ");
        Serial.print(currentFrequencyMHz); // Print with 3 decimal places
        Serial.println(" MHz");

        // Debouncing
        delay(300);
    }
}


void TM1638_handleFrequencyDown() {
    uint8_t buttonsValue = tm.readButtons();

    // Check if button 5 is pressed (Frequency Down)
    if (buttonsValue & 0x10) { // 0x20 = bit for button 5
        si4735.frequencyDown();  // Decrease the frequency


        // Retrieve the current frequency and convert to MHz
        uint16_t currentFrequency = si4735.getFrequency(); // Assuming getFrequency() exists
        float currentFrequencyMHz = currentFrequency / 100.0; // Convert kHz to MHz


        tm.displayText("        ");  // Ștergem textul precedent
        // Display the frequency on the TM1638 display
        String displayText = "FRE " + String(currentFrequencyMHz, 1); // Display with 1 decimal places
        tm.displayText(displayText.c_str());  // Convert to const char* and display

        // Print to serial
        Serial.print("Current Frequency: ");
        Serial.print(currentFrequencyMHz); // Print with 3 decimal places
        Serial.println(" MHz");

        // Debouncing
        delay(300);
    }
}


// Structura pentru stocarea posturilor de radio
struct RadioStation {
  String name;
  float frequency;  // Frecvența în MHz
};

// Vector pentru stocarea posturilor de radio
std::vector<RadioStation> radioStations;
int currentStationIndex = 0;  // Indexul postului de radio curent

// Funcția pentru citirea fișierului CSV și încărcarea posturilor de radio
void loadRadioStations() {
  File file = LittleFS.open("/Radio_fm/FM_Bucuresti.csv", "r");
  if (!file) {
    Serial.println("Eroare la deschiderea fișierului");
    return;
  }

  while (file.available()) {
    String line = file.readStringUntil('\n');
    int firstComma = line.indexOf(',');
    int secondComma = line.indexOf(',', firstComma + 1);

    // Extragem numele postului și frecvența
    String stationName = line.substring(0, firstComma);
    String frequencyStr = line.substring(firstComma + 1, secondComma);
    float frequency = frequencyStr.toFloat();

    // Adăugăm postul de radio în vector
    radioStations.push_back(RadioStation{stationName, frequency});
  }

  file.close();
  Serial.println("Posturile de radio au fost încărcate.");
}



// Funcția pentru schimbarea frecvenței la postul de radio curent
void changeFrequency() {
  if (radioStations.empty()) {
    Serial.println("Nu sunt posturi de radio disponibile.");
    return;
  }

  // Obținem frecvența postului curent (în MHz)
  float frequencyMHz = radioStations[currentStationIndex].frequency;

  // Convertim frecvența în kHz (înmulțim cu 100)
  float frequencyKHz = frequencyMHz * 100;

  // Setăm frecvența pe radio
  si4735.setFrequency(frequencyKHz);  // Setăm frecvența în kHz

  // Afișăm frecvența curentă
  Serial.print("Frecvența curentă: ");
  Serial.print(frequencyMHz);
  Serial.println(" MHz");

  // Actualizăm display-ul cu frecvența
  tm.displayText("        ");  // Ștergem textul precedent
  String displayText = "FRE " + String(frequencyMHz, 1); // Afișăm cu 1 zecimală
  tm.displayText(displayText.c_str());  // Convertim în const char* și afișăm
}

// Funcția pentru butonul 6 (frecvență up)
void TM1638_CSV_handleFrequencyUp() {
  uint8_t buttonsValue = tm.readButtons();

  // Verificăm dacă butonul 6 este apăsat (frecvență up)
  if (buttonsValue & 0x20) {  // 0x20 = bitul 6
    if (currentStationIndex < radioStations.size() - 1) {
      currentStationIndex++;  // Mergem la următorul post de radio
      changeFrequency();
    } else {
      Serial.println("Nu mai există posturi de radio.");
    }

    // Debouncing simplu
    delay(300);
  }
}

// Funcția pentru butonul 7 (frecvență down)
void TM1638_CSV_handleFrequencyDown() {
  uint8_t buttonsValue = tm.readButtons();

  // Verificăm dacă butonul 7 este apăsat (frecvență down)
  if (buttonsValue & 0x40) {  // 0x40 = bitul 7
    if (currentStationIndex > 0) {
      currentStationIndex--;  // Mergem la postul de radio anterior
      changeFrequency();
    } else {
      Serial.println("Nu mai există posturi de radio.");
    }

    // Debouncing simplu
    delay(300);
  }
}



void displayCurrentFrequency() {
    if (si4735.isCurrentTuneFM()) {
        // Obține frecvența curentă în kHz și convertește în MHz
        float frequencyMHz = si4735.getFrequency() / 100.0;

        // Formatează textul pentru afișaj
        char displayText[10];
        snprintf(displayText, sizeof(displayText), "FRE %04.1f", frequencyMHz); // Afișăm cu 1 zecimală

        // Actualizează display-ul TM1638
        tm.displayText(displayText);  // Afișează textul

        // Afișează și pe Serial Monitor
        Serial.print("Frecvența curentă: ");
        Serial.print(frequencyMHz, 1);
        Serial.println(" MHz");
    } else {
        Serial.println("Nu există o frecvență FM curentă.");
    }
}


void TM1638_displayWaitingDotsAndLEDs() {
    static int numDots = 0;       // Numărul de puncte afișate (static pentru a reține valoarea între apeluri)
    static int currentLED = 0;    // LED-ul curent aprins

    // Construim șirul pentru afișaj
    char displayText[9] = "        "; // Inițializăm cu spații
    for (int i = 0; i < numDots; i++) {
        displayText[i] = '.'; // Adăugăm puncte în funcție de numDots
    }

    // Afișăm textul pe TM1638
    tm.displayText(displayText);

    // Aprindem LED-ul curent
    tm.setLED(currentLED, 1);
    delay(500); // Pauză pentru efect

    // Stingem LED-ul anterior
    tm.setLED(currentLED, 0);

    // Incrementăm punctele și LED-ul
    numDots = (numDots + 1) % 9;   // Resetăm la 0 după 8 puncte
    currentLED = (currentLED + 1) % 8; // Resetăm la LED-ul 0 după LED-ul 7
}











bool backlightState = true; // Inițial, backlight-ul este aprins
uint8_t currentBrightness = 0;  // Inițializare la 0 (luminozitate minimă)

void TM1638_handleTestButton() {
  uint8_t buttonsValue = tm.readButtons();
  static unsigned long buttonPressStartTime = 0; // Momentul când butonul este apăsat
  static bool isLongPress = false;              // Indicator pentru apăsare lungă

  // Verificăm dacă butonul 8 este apăsat
  if (buttonsValue & 0x80) {  // 0x80 = bitul 8
    if (buttonPressStartTime == 0) {
      // Înregistrăm momentul când butonul este apăsat
      buttonPressStartTime = millis();
      isLongPress = false;
    } else if (millis() - buttonPressStartTime > 3000 && !isLongPress) {
      // Dacă butonul este apăsat mai mult de 3 secunde
      isLongPress = true; // Marcăm că este o apăsare lungă
      // Comutăm starea backlight-ului
      backlightState = !backlightState;   // Inversăm starea
      digitalWrite(TFT_BL, backlightState ? TFT_BACKLIGHT_ON : !TFT_BACKLIGHT_ON);
    }
  } else {
    // Dacă butonul nu mai este apăsat
    if (buttonPressStartTime > 0 && !isLongPress) {
      // Funcționalitatea normală dacă nu a fost o apăsare lungă
      if (currentBrightness == 7) {
        tm.displayText("        ");  // Stingem display-ul
        currentBrightness = 0;      // Resetăm luminozitatea la minim
      } else {
        // Creștem luminozitatea
        currentBrightness++;
        tm.brightness(currentBrightness);  // Aplicăm luminozitatea
      }
    }
    // Resetăm temporizatorul
    buttonPressStartTime = 0;
  }
}






/*
// Funcția care gestionează schimbarea luminozității la fiecare apăsare de buton
// Variabila pentru luminozitate curentă
uint8_t currentBrightness = 0;  // Inițializare la 0 (luminozitate minimă)

// Funcția care gestionează schimbarea luminozității la fiecare apăsare de buton
void TM1638_handleTestButton() {
  uint8_t buttonsValue = tm.readButtons();

  // Verificăm dacă butonul 8 este apăsat
  if (buttonsValue & 0x80) {  // 0x80 = bitul 8
    // Dacă luminozitatea a ajuns la maxim (7), o resetăm la minim (0) și stingem display-ul
    if (currentBrightness == 7) {
      tm.displayText("        ");  // Stingem display-ul
      currentBrightness = 0;  // Resetăm luminozitatea la minim
    } else {
      // Creștem luminozitatea
      currentBrightness++;  
      tm.brightness(currentBrightness);  // Aplicăm luminozitatea

    }

    // Debouncing simplu
    delay(300);
  }
}


*/