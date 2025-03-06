
#include "RDS.h"
#include <Arduino.h>

// Global instance of the SI4735 radio object
extern SI4735 si4735;

// Buffer to store the last RDS message
char lastRDSMessage[64] = ""; // Example size

// Function to handle RDS information
void rds() {
    // Check RDS status
    si4735.getRdsStatus();

    // Ensure RDS is received and synchronized
    if (si4735.getRdsReceived() && si4735.getRdsSync() && si4735.getRdsSyncFound()) {
        const char* rdsMessage = si4735.getRdsText0A(); // Get the RDS message (Program Service Name)
        
        // Check if the RDS message is valid and new
        if (rdsMessage != nullptr && strlen(rdsMessage) > 0 && strcmp(lastRDSMessage, rdsMessage) != 0) {
            strncpy(lastRDSMessage, rdsMessage, sizeof(lastRDSMessage) - 1);
            lastRDSMessage[sizeof(lastRDSMessage) - 1] = '\0'; // Ensure null termination
            Serial.print("RDS Message: ");
            Serial.println(lastRDSMessage);
        }
    }
}

// Function to display the UTC time
void displayUtcTime() {
  // Verifică dacă sunt disponibile date RDS
  if (si4735.getNumRdsFifoUsed() > 0) {
    // Obține timpul UTC din RDS
    char *utcTime = si4735.getRdsDateTime();

    // Verifică dacă timpul UTC a fost obținut corect
    if (utcTime != NULL) {
      // Extrage doar partea de timp (HH:MM)
      char timeOnly[6];  // Buffer pentru ora (HH:MM)
      strncpy(timeOnly, utcTime + 11, 5);  // Extrage caracterele de la poziția 11 (ora)
      timeOnly[5] = '\0';  // Adaugă terminatorul de șir

      // Afișează timpul
      Serial.print("Timp UTC: ");
      Serial.println(timeOnly);
    }
  }
}






void rds_all_info() {
  char *utcTime = nullptr;
  char *stationName = nullptr;
  char *programInfo = nullptr;
  char *stationInfo = nullptr;

  if (si4735.isCurrentTuneFM()) {
    // Populează datele RDS
    if (si4735.getRdsAllData(&stationName, &stationInfo, &programInfo, &utcTime)) {
      // Afișează informațiile despre program
      if (programInfo != nullptr) {
        // Afișare sau procesare informații despre program
        Serial.println("Program Info: ");
        Serial.println(programInfo);
      }

      // Afișează numele stației
      if (stationName != nullptr) {
        Serial.println("Station Name: ");
        Serial.println(stationName);
      }

      // Afișează informațiile stației
      if (stationInfo != nullptr) {
        Serial.println("Station Info: ");
        Serial.println(stationInfo);
      }

      // Afișează timpul UTC
      if (utcTime != nullptr) {
        Serial.println("UTC Time: ");
        Serial.println(utcTime);
      }
    }
  }
}


void getCurrentReceivedSignalQuality() {
  // Apelăm metoda pentru a obține statusul RSQ (Received Signal Quality)
  si4735.getCurrentReceivedSignalQuality(1); // INTACK = 1 pentru a reseta întreruperile

  // Obținem și afișăm informațiile despre calitatea semnalului
  int rssi = si4735.getCurrentRSSI();  // Received Signal Strength Indicator
  int snr = si4735.getCurrentSNR();    // Signal-to-Noise Ratio

  // Afișăm rezultatele în Serial Monitor
  Serial.println("Received Signal Quality:");
  Serial.print("  RSSI (Signal Strength): ");
  Serial.print(rssi);
  Serial.println(" dBµV");

  Serial.print("  SNR (Signal-to-Noise Ratio): ");
  Serial.print(snr);
  Serial.println(" dB");

}
