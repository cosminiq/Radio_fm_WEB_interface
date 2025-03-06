
#include "meniu_serial.h"

MeniuNivel nivelCurent = MENIU_PRINCIPAL;

// Funcția pentru a afișa ajutorul
void showHelp() {
  Serial.println("Type F to FM; A to AM");
  Serial.println("Type U to increase and D to decrease the frequency");
  Serial.println("Type S or s to seek station Up or Down");
  Serial.println("Type + or - to volume Up or Down");
  Serial.println("Type Q to show current status");
  Serial.println("Type B to change Bandwidth filter");
  Serial.println("Type M or m mute or unmute radio");
  Serial.println("Type ? to this help.");
  Serial.println("==================================================");
  delay(1000);
}

// Funcția pentru a afișa meniul principal
void showHelp_main_menu() {
  Serial.println("Meniu activat. Alege o optiune:");
  Serial.println("--------------------------------------------------");
  
  Serial.println("1 - Handle Serial Input");
  Serial.println("   Aceasta optiune permite manipularea intrarilor seriale, cum ar fi controlul volumului, schimbarea frecventei, cautarea unei statii, mute/unmute si altele.");
  Serial.println("   Dupa selectarea acestei optiuni, vei putea introduce comenzi prin terminalul serial.");
  Serial.println();
  
  Serial.println("2 - Handle Frequency Input");
  Serial.println("   Aceasta optiune permite introducerea manuala a unei frecvente FM.");
  Serial.println("   Introduceti frecventa dorita in kHz (ex: 9000 pentru 90.0 MHz).");
  Serial.println("   Frecventa va fi setata pe radio si va fi afisata pe ecran.");
  Serial.println();
  
  Serial.println("3 - Alegerea predefinita a posturilor");
  Serial.println("   Aceasta optiune iti permite sa selectezi rapid un post FM predefinit.");
  Serial.println("   Vei putea alege dintr-o lista de posturi preconfigurate, fiecare avand o frecventa specifica.");
  Serial.println("   Introdu numarul sau litera asociata postului dorit pentru a-l selecta.");
  Serial.println();
  
  Serial.println("Q - Iesire din meniu");
  Serial.println("   Aceasta optiune te va duce inapoi la meniul principal sau va iesi din meniul curent.");
  Serial.println("   Foloseste aceasta optiune daca doresti sa iesi din meniul curent.");
  Serial.println();
}

// Funcția pentru a afișa ajutorul pentru submeniul Handle Frequency Input
void showHelp_handleFrequencyInput() {
  Serial.println("==================================================");
  Serial.println("Meniu de introducere a frecvenței FM:");
  Serial.println("1. Introduceți frecvența FM dorită în kHz.");
  Serial.println("2. Frecvențele valide sunt între 8750 kHz și 10800 kHz.");
  Serial.println("   De exemplu: 9000 pentru 90.00 MHz sau 10060 pentru 100.60 MHz.");
  Serial.println("3. Comenzi disponibile:");
  Serial.println("   - Apăsați F sau f pentru a începe introducerea frecvenței.");
  Serial.println("   - Apăsați X sau x pentru a reveni la meniul principal.");
  Serial.println("4. Exemple de frecvențe valabile:");
  Serial.println("   - 8750 kHz (87.5 MHz)");
  Serial.println("   - 10000 kHz (100.00 MHz)");
  Serial.println("   - 10800 kHz (108.0 MHz)");
  Serial.println("==================================================");
  delay(1000);
}

// Funcția pentru a afișa ajutorul pentru submeniul Posturi FM
void showHelp_handleSubmeniuPosturiFM() {
  Serial.println("--------------------------------------------------");
  Serial.println("3 - Handle Submeniu Posturi FM");
  Serial.println("Aceasta optiune permite selectarea unui post FM predefinit dintr-o lista.");
  Serial.println();
  Serial.println("Functionalitati disponibile:");
  Serial.println(" - Introdu un numar intre 1 si " + String(numarPosturi) + " pentru a selecta postul FM corespunzator.");
  Serial.println("   Posturile disponibile sunt predefinite si includ frecvente comune.");
  Serial.println(" - Apasa 'W' pentru a afisa instructiuni suplimentare despre cum sa utilizezi acest submeniu.");
  Serial.println(" - Apasa 'X' pentru a reveni la meniul principal.");
  Serial.println();
  Serial.println("Exemple:");
  Serial.println(" - Daca introduci '1-9' si 'A-S', se vor selecta posturi din lista.");
  Serial.println(" - Daca introduci o valoare invalida, vei primi un mesaj de eroare.");
  Serial.println("--------------------------------------------------");
}

// Funcția pentru a gestiona meniul principal
void handleMeniuPrincipal(char input) {
  switch (input) {
    case 'M':
    case 'm':
      showHelp_main_menu();
      break;
    case '1':
      nivelCurent = SUBMENIU_SERIAL;  // Trecem la submeniul Serial
      Serial.println("Ati intrat in submeniul Handle Serial Input.");
      showHelp();
      Serial.println("Apasa X pentru a reveni la meniul principal.");
      break;
    case '2':
      nivelCurent = SUBMENIU_FREQUENCY;  // Trecem la submeniul Frequency
      showHelp_handleFrequencyInput();
      break;
    case '3':
      nivelCurent = SUBMENIU_FM_BUCURESTI;
      showHelp_handleSubmeniuPosturiFM();
      showPosturiFM(); // Afișează lista posturilor FM
      break;
    case 'Q':
    case 'q':
      Serial.println("Meniu inchis.");
      break;
    default:
      Serial.println("Optiune invalida. Alege 1, 2 sau Q.");
      break;
  }
}

// Funcția pentru a gestiona submeniul Handle Serial Input
void handleSubmeniuSerial(char input) {
  switch (input) {
    case '+': handleVolumeUp(); break;
    case '-': handleVolumeDown(); break;
    case 'U': case 'u': handleFrequencyUp(); break;
    case 'D': case 'd': handleFrequencyDown(); break;
    case 'S': handleSeekStationUp(); break;
    case 's': handleSeekStationDown(); break;
    case 'Q': showStatus(); break;
    case 'M': muteState = true; handleMuteUnmute(); break;
    case 'm': muteState = false; handleMuteUnmute(); break;
    case 'b': handleBandwidthChange(); break;
    case 'a': case 'A': handleAMFMSelection(input); break;
    case 'f': case 'F': handleAMFMSelection(input); break;
    case '?': showHelp(); break;
    case 'X': case 'x':
      nivelCurent = MENIU_PRINCIPAL;  // Revenim la meniul principal
      Serial.println("Revenire la meniul principal.");
      break;
    default:
      Serial.println("Comanda invalida. Apasa X pentru a reveni la meniul principal.");
      break;
  }
}

// Funcția pentru a gestiona submeniul Handle Frequency Input
void handleSubmeniuFrequency(char input) {
  switch (input) {
    case 'F':
    case 'f':
      handleFrequencyInput();  // Permite introducerea manuală a frecvenței
      break;
    case 'X':
    case 'x':
      nivelCurent = MENIU_PRINCIPAL;  // Revenim la meniul principal
      Serial.println("Revenire la meniul principal.");
      break;
    default:
      Serial.println("Comanda invalida. Apasa F pentru introducerea frecventei sau X pentru a reveni la meniul principal.");
      break;
  }
}

// Funcția pentru a gestiona submeniul Handle Submeniu Posturi FM
void handleSubmeniuPosturiFM(char input) {
  switch (input) {
    case 'W':
    case 'w':
      Serial.println("Introdu un număr valid pentru a selecta un post FM predefinit.");
      break;
    case 'X':
    case 'x':
      nivelCurent = MENIU_PRINCIPAL; // Revenire la meniul principal
      Serial.println("Revenire la meniul principal.");
      break;
    default:
      selecteazaPostDupaInput(input);
      break;
  }
}


