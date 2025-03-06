#include "Timer_ntp.h"

void TM1638_displayNTPTime() {
    const char* ntpServer = "time.google.com";
    const long gmtOffset_sec = 2 * 3600; // GMT+2 (România)
    const int daylightOffset_sec = 3600; // Ora de vară

    // Configurarea NTP
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    Serial.println("NTP configurat.");

    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
        Serial.println("Eroare: Nu s-a putut obține timpul NTP.");
        return;
    }

    // Afișarea timpului în format HH MM
    char displayText[9]; // Pentru a stoca textul de afișat (formatul __HHMM__)
    snprintf(displayText, sizeof(displayText), "_%02d__%02d_", timeinfo.tm_hour, timeinfo.tm_min);

    // Afișarea pe display
    tm.displayText(displayText);  // Display-ul va arăta ora și minutele în format militar

    // Afișarea timpului și pe serial monitor
    Serial.printf("Ora curentă: %02d:%02d:%02d, Data: %02d-%02d-%04d\n",
                  timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec,
                  timeinfo.tm_mday, timeinfo.tm_mon + 1, timeinfo.tm_year + 1900);
}
