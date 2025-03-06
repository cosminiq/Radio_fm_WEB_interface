#include "posturi_fm_B.h"
#include <LittleFS.h>  // Biblioteca LittleFS pentru gestionarea fișierelor pe flash

PostFM posturiFM[100];  // Presupunem că vor fi maximum 100 de posturi FM
int numarPosturi = 0;


// Funcția pentru a inițializa posturile FM
void initPosturiFM() {
  if (!LittleFS.begin()) {
    Serial.println("Eroare la inițializarea LittleFS!");
    return;
  }
  Serial.println("LittleFS inițializat cu succes!");

  File file = LittleFS.open("/Radio_fm/FM_Bucuresti.csv", "r");
  if (!file) {
    Serial.println("Eroare la deschiderea fișierului!");
    return;
  }
  Serial.println("Fișier deschis cu succes!");

  int linesRead = 0;

  while (file.available()) {
    String line = file.readStringUntil('\n');
    linesRead++;  // Incrementăm numărul de linii citite

    int firstComma = line.indexOf(',');
    int secondComma = line.indexOf(',', firstComma + 1);

    if (firstComma != -1 && secondComma != -1) {
      String nume = line.substring(0, firstComma);
      float frecventa = line.substring(firstComma + 1, secondComma).toFloat();
      String descriere = line.substring(secondComma + 1);

      posturiFM[numarPosturi].nume = nume;
      posturiFM[numarPosturi].frecventa = frecventa;
      posturiFM[numarPosturi].gen = descriere;

      numarPosturi++;
    }
  }

  file.close();
  Serial.print("Posturi FM încărcate: ");
  Serial.println(numarPosturi);
  Serial.print("Linii citite din CSV: ");
  Serial.println(linesRead);
}


// Funcția pentru a afișa posturile FM
void showPosturiFM() {
  Serial.println("Posturi FM disponibile (introduceți numărul corespunzător):");
  for (int i = 0; i < numarPosturi; i++) {
    if (i < 9) {
      Serial.print(i + 1);
    } else {
      Serial.print(char('A' + (i - 9)));  // Correct the mapping
    }
    Serial.print(". ");
    Serial.print(posturiFM[i].nume);
    Serial.print(" - ");
    Serial.print(posturiFM[i].frecventa);
    Serial.println(" MHz");
  }
  Serial.println("X . Revenire la meniul principal");
}

// Funcția pentru a selecta un post FM
void selecteazaPostFM(float selectedFrequency) {
  currentFrequency = selectedFrequency * 100;  // Convertim în kHz
  si4735.setFM(currentFrequency, currentFrequency + 200, 10060, 10); // Setăm intervalul de frecvență
  si4735.setRdsConfig(1, 3, 3, 3, 3);  // Activăm RDS
  si4735.setGpioCtl(1, 0, 0); // Unmute radio
  si4735.setGpio(1, 0, 0);  // Unmute radio

  Serial.print("Tuned to FM Frequency: ");
  Serial.print(selectedFrequency);
  Serial.println(" MHz");
}

bool selecteazaPostDupaInput(char input) {
  int index;

  // Verificăm dacă intrarea este o cifră (1-9)
  if (isdigit(input)) {
    index = input - '1';  // Convertim caracterul într-un index numeric
  }
  // Verificăm dacă intrarea este o literă (A-Z)
  else if (isalpha(input)) {
    input = toupper(input);  // Convertim la majusculă
    index = input - 'A' + 9;  // Mapăm literele la indexuri începând de la 9
  }
  // Dacă intrarea este invalidă
  else {
    Serial.println("Selecție invalidă. Introduceți un număr valid sau apăsați X pentru a reveni la meniul principal.");
    return false;
  }

  // Verificăm dacă indexul este valid
  if (index >= 0 && index < numarPosturi) {
    float selectedFrequency = posturiFM[index].frecventa;
    selecteazaPostFM(selectedFrequency);  // Setăm postul FM selectat
    return true;
  } else {
    Serial.println("Selecție invalidă. Introduceți un număr valid sau apăsați X pentru a reveni la meniul principal.");
    return false;
  }
}