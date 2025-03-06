#include "serial_function.h"
#include "globals.h"
#include <Wire.h>
#include <SI4735.h>

/**
 * Afișează starea curentă a tunerului radio, inclusiv frecvența, calitatea semnalului și alte informații relevante.
 * Utilitate: Permite utilizatorului să vadă detalii despre stația curentă, cum ar fi frecvența, SNR (raport semnal-zgomot),
 * RSSI (puterea semnalului) și dacă semnalul este stereo sau mono.
 */
void showStatus() {
  si4735.getStatus();
  si4735.getCurrentReceivedSignalQuality();
  Serial.print("You are tuned on ");
  
  if (si4735.isCurrentTuneFM()) {
    Serial.print(String(currentFrequency / 100.0, 2));  // Frecvența FM
    Serial.print(" MHz ");
    
    // Afișează dacă semnalul este stereo sau mono
    Serial.print((si4735.getCurrentPilot()) ? "STEREO" : "MONO");
    
    // Afișează informații suplimentare despre semnal
    Serial.print(" [SNR: ");
    Serial.print(si4735.getCurrentSNR());  // Signal-to-Noise Ratio
    Serial.print(" dB, RSSI: ");
    Serial.print(si4735.getCurrentRSSI());  // Received Signal Strength Indicator
    Serial.print(" dBuV]");
    
  } else {
    Serial.print(currentFrequency);  // Frecvența AM
    Serial.print(" kHz");
  }
  
  // Afișează alte informații utile, de exemplu, starea tunerului
  Serial.print(" [Status: ");
  if (si4735.isCurrentTuneFM()) {
    Serial.print("FM");
  } else {
    Serial.print("AM");
  }
  Serial.print("]");
  
  Serial.println();  // Încheie linia

}

/**
 * Controlează starea de mute/unmute a radio.
 * Utilitate: Permite utilizatorului să dezactiveze sau să activeze sunetul radio.
 */
void handleMuteUnmute() {
  if (muteState) {
    si4735.setGpio(1, 0, 0);  // Unmute radio
    Serial.println("Radio Unmuted");
  } else {
    si4735.setGpio(0, 0, 0);  // Mute radio
    Serial.println("Radio Muted");
  }
}

/**
 * Crește volumul radio.
 * Utilitate: Permite utilizatorului să mărească volumul sunetului.
 */
void handleVolumeUp() {
  si4735.volumeUp();
}

/**
 * Scade volumul radio.
 * Utilitate: Permite utilizatorului să micșoreze volumul sunetului.
 */
void handleVolumeDown() {
  si4735.volumeDown();
}

/**
 * Crește frecvența radio și activează RDS (Radio Data System).
 * Utilitate: Permite utilizatorului să sintonizeze stații la frecvențe mai înalte și să afișeze frecvența curentă.
 */
void handleFrequencyUp() {
  si4735.frequencyUp();
  si4735.setRdsConfig(1, 3, 3, 3, 3);  // Enable RDS
  // Retrieve and print the current frequency
  uint16_t currentFrequency = si4735.getFrequency(); // Assuming getFrequency() exists
  Serial.print("Current Frequency: ");
  Serial.print(currentFrequency);
  Serial.println(" kHz");
  
}

/**
 * Scade frecvența radio și activează RDS (Radio Data System).
 * Utilitate: Permite utilizatorului să sintonizeze stații la frecvențe mai joase și să afișeze frecvența curentă.
 */
void handleFrequencyDown() {
  si4735.frequencyDown();
  si4735.setRdsConfig(1, 3, 3, 3, 3);  // Enable RDS
    // Retrieve and print the current frequency
  uint16_t currentFrequency = si4735.getFrequency(); // Assuming getFrequency() exists
  Serial.print("Current Frequency: ");
  Serial.print(currentFrequency);
  Serial.println(" kHz");
}

/**
 * Caută următoarea stație radio în direcția crescătoare a frecvenței.
 * Utilitate: Permite utilizatorului să găsească rapid stații disponibile fără a introduce manual frecvențe.
 */
void handleSeekStationUp() {
  si4735.seekStationUp();
}

/**
 * Caută următoarea stație radio în direcția descrescătoare a frecvenței.
 * Utilitate: Permite utilizatorului să găsească rapid stații disponibile fără a introduce manual frecvențe.
 */
void handleSeekStationDown() {
  si4735.seekStationDown();
}


/**
 * Schimbă lățimea de bandă a filtrului radio.
 * Utilitate: Permite utilizatorului să ajusteze lățimea de bandă pentru a îmbunătăți calitatea semnalului
 * în funcție de condițiile de recepție.
 */
void handleBandwidthChange() {
  static uint8_t bandwidthIdx = 0;
  const char *bandwidth[] = {"6", "4", "3", "2", "1", "1.8", "2.5"};
  if (bandwidthIdx > 6) bandwidthIdx = 0;
  si4735.setBandwidth(bandwidthIdx, 1);
  Serial.print("Filter - Bandwidth: ");
  Serial.print(String(bandwidth[bandwidthIdx]));
  Serial.println(" kHz");
  bandwidthIdx++;
}

/**
 * Selectează între modul AM și FM în funcție de tasta apăsată de utilizator.
 * Utilitate: Permite utilizatorului să comute între benzile AM și FM și să seteze frecvențe specifice pentru fiecare bandă.
 */
void handleAMFMSelection(char key) {
  if (key == 'a' || key == 'A') {
    si4735.setAM(570, 1710, 810, 10);
  } else if (key == 'f' || key == 'F') {
    si4735.setFM(8600, 10800, 10060, 10);
    si4735.setRdsConfig(1, 3, 3, 3, 3);  // Enable RDS
    si4735.setGpioCtl(1, 0, 0); // Unmute radio
    si4735.setGpio(1, 0, 0);  // Unmute radio
  }
}

/**
 * Verifică dacă un șir de caractere este format doar din cifre.
 * Utilitate: Folosit pentru validarea intrărilor utilizatorului, asigurându-se că frecvențele introduse sunt numerice.
 */
bool isNumeric(const String& str) {
  for (unsigned int i = 0; i < str.length(); i++) {
    if (!isDigit(str[i])) return false;  // Verifică dacă fiecare caracter este o cifră
  }
  return true;  // Returnează true dacă toate caracterele sunt cifre
}

/**
 * Permite utilizatorului să introducă manual o frecvență FM și să sintonizeze radio pe acea frecvență.
 * Utilitate: Oferă flexibilitate utilizatorului de a selecta manual o frecvență specifică, cu validare
 * pentru a asigura că frecvența introdusă este în intervalul valid pentru FM.
 */
void handleFrequencyInput() {
  // Curăță bufferul serial pentru a evita citirea caracterelor rămase
  while (Serial.available() > 0) {
    Serial.read(); // Golim bufferul
  }

  Serial.println("Enter the FM frequency (in kHz):");

  // Așteptăm până când există o intrare validă
  while (!Serial.available());

  String inputFrequency = Serial.readStringUntil('\n');
  inputFrequency.trim(); // Eliminăm spațiile sau caracterele inutile

  if (inputFrequency.length() == 0 || !isNumeric(inputFrequency)) {
    Serial.println("Invalid input. Please enter a valid frequency.");
    return;
  }

  long freq = inputFrequency.toInt();

  if (freq >= 8750 && freq <= 10800) {
    currentFrequency = freq;
    si4735.setFM(freq, freq + 200, 10060, 10);
    si4735.setRdsConfig(1, 3, 3, 3, 3);  // Enable RDS
    si4735.setGpioCtl(1, 0, 0); // Unmute radio
    si4735.setGpio(1, 0, 0);  // Unmute radio
    Serial.print("Tuned to FM Frequency: ");
    Serial.print(freq / 100.0, 2);
    Serial.println(" MHz");
  } else {
    Serial.println("Invalid FM frequency. Please enter a valid frequency between 8750 kHz and 10800 kHz.");
  }
}
