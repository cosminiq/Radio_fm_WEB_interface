#include "globals.h"

#include "ploting_cpu.h"




// Definirea funcțiilor pentru taskuri
void taskMeniu(void *pvParameters);          // Task pentru meniuri
void taskRds(void *pvParameters);            // Task pentru RDS
void taskWebServer(void *pvParameters);      // Task pentru serverul web
void taskNTPTimeDisplay(void *pvParameters); // Declarația funcției task-ului
void taskTM1638Control(void *pvParameters);  // Task pentru controlul TM1638
void taskFetchWeatherData(void *pvParameters); // Task pentru actualizarea datelor meteo
void taskAlternateImages(void *pvParameters); // Task pentru alternarea imaginilor
void taskCheckFrequency(void *pvParameters); 
//void monitorResources(void *pvParameters); // Task pentru monitorizarea resurselor
void taskUpdatePlot(void *pvParameters); // Task pentru actualizarea plotului


bool isPlotUpdateRunning = false; // Inițializare flag
bool isSerialMonitorRunning = false; // Inițializare flag

SemaphoreHandle_t mutex;
SemaphoreHandle_t plotUpdateSemaphore = NULL; // Inițializare semaforn pentru plot
SemaphoreHandle_t meniuSemaphore = NULL;  // Semafor pentru task-ul Meniu



void setup() 
{
  Serial.begin(115200); // Inițializăm comunicarea serială
  while (!Serial)
    ; // Așteptăm să se stabilească conexiunea serială
      // Initialize TM1638
  setupTM1638();

  // Inițializează mutex-ul
  mutex = xSemaphoreCreateMutex();
  plotUpdateSemaphore = xSemaphoreCreateBinary(); // Creează semaforul binar
  xSemaphoreGive(plotUpdateSemaphore); // Eliberează semaforul inițial (permite execuția altor task-uri)

  meniuSemaphore = xSemaphoreCreateBinary();  // Creează semaforul pentru task-ul Meniu
  xSemaphoreGive(meniuSemaphore);  // Eliberează semaforul pentru a permite task-ului să ruleze




  tft.init();
  tft.setRotation(0);
  tft.fillScreen(TFT_BLACK);
  pinMode(TFT_BL, OUTPUT); // Configurăm pinul TFT_BL ca OUTPUT

  // Configurăm pinul de reset pentru modulul radio
  digitalWrite(RESET_PIN, HIGH);
  Serial.println("AM and FM station tuning test.");
  Wire.begin(ESP32_I2C_SDA, ESP32_I2C_SCL); // Inițializăm comunicația I2C

  delay(500);                           // Așteptăm 500 ms
  si4735.setup(RESET_PIN, FM_FUNCTION); // Configurăm modulul radio pentru FM
  si4735.setFM(8400, 10800, 10060, 10); // Setăm intervalul de frecvențe FM
  si4735.setRdsConfig(1, 3, 3, 3, 3);   // Activăm RDS (Radio Data System)

  delay(500);                                                   // Așteptăm 500 ms pentru a ne asigura că totul este configurat
  currentFrequency = previousFrequency = si4735.getFrequency(); // Obținem frecvența curentă
  si4735.setVolume(10);                                         // Setăm volumul la 25
  showStatus();                                                 // Afișăm statusul radio
  si4735.setGpioCtl(1, 0, 0);                                   // Setăm controlul GPIO
  si4735.setGpio(1, 0, 0);                                      // Dezactivăm mute pe radio


    // Inițializare sprite
  graphSprite.createSprite(PLOT_WIDTH, PLOT_HEIGHT); // Creează un sprite cu dimensiunile zonei de plotare
  graphSprite.fillSprite(TFT_BLACK); // Umple sprite-ul cu negru
  textSprite.createSprite(110, 40); // Lungimea și înălțimea sprite-ului pentru sprite-ul in care se afișează textul
  textSprite.fillSprite(TFT_BLACK); // Umple sprite-ul cu negru





  citesteFisierCSV();   // Funcția pentru a citi și a arăta conținutul fișierului CSV sub forma unui tabel
  scanAndConnectWiFi(); // Funcție pentru scanarea rețelelor și conectare
  initPosturiFM();      // Inițializăm posturile FM din fișier
  //showHelp_main_menu(); // Afișăm ajutorul pentru meniul principal
  loadRadioStations();  // Funcția pentru a încărca posturile radio din fișierul CSV
  displayUtcTime();     // Afișăm timpul UTC


  


  /*
  Creăm taskurile pentru FreeRTOS
  Rezumat:
  taskMeniu: Funcția care va rula în cadrul taskului.
  "Task Meniu": Numele taskului (pentru referință).
  2048: Memoria alocată pentru stiva taskului (2048 de octeți).
  NULL: Parametrii suplimentari pentru task (în acest caz, nu sunt necesari).
  1: Prioritatea taskului (nivelul 1).
  NULL: Handle-ul taskului (nu este necesar în acest caz).
  0: Nucleul pe care va rula taskul (Core 0)
  */

  xTaskCreatePinnedToCore(taskMeniu, "Task Meniu", 2048, NULL, 1, NULL, 0);          // Task pentru meniuri pe Core 0
  xTaskCreatePinnedToCore(taskRds, "Task RDS", 2048, NULL, 1, NULL, 0);              // Task pentru RDS pe Core 0
  xTaskCreatePinnedToCore(taskWebServer, "Task Web Server", 2048, NULL, 1, NULL, 1); // Task pentru serverul web pe Core 1
  xTaskCreatePinnedToCore(taskNTPTimeDisplay, "Task NTP Time Display", 2048, NULL, 1, NULL, 0); // Task pentru afișarea timpului NTP pe Core 0
  xTaskCreatePinnedToCore(taskTM1638Control, "Task TM1638 Control", 2048, NULL, 1, NULL, 0); // Task pentru controlul TM1638 pe Core 0
  xTaskCreatePinnedToCore(taskAlternateImages, "Task Alternate Images", 8192, NULL, 1, NULL, 1);  // Crează și lansează task-ul pentru alternarea imaginilor
  xTaskCreatePinnedToCore(taskFetchWeatherData, "Fetch Weather Data", 4096, NULL, 1, NULL, 0);// Creează task-ul pentru actualizarea datelor meteo pe Core 0
  xTaskCreatePinnedToCore(taskCheckFrequency, "Task Check Frequency", 2048, NULL, 1, NULL, 1);  // Task pentru verificarea frecvenței pe Core 1

    // Creare task pentru actualizarea plotului pe Core 1
  xTaskCreatePinnedToCore(
      taskUpdatePlot,    // Funcția task-ului
      "UpdatePlotTask",  // Numele task-ului
      2048,              // Dimensiunea stivei
      NULL,              // Parametrii pentru task
      1,                 // Prioritatea task-ului
      NULL,              // Handle-ul task-ului
      1                  // Nucleul pe care rulează task-ul (Core 1)
  );


}















// Task pentru serverul web
void taskWebServer(void *pvParameters)
{
  // Inițializăm serverul web într-un task separat
  Server_ESP_32_Control_FM_WEB(); // Apelăm funcția care inițializează serverul web
  while (true)
  {
    delay(1000); // Pauză pentru a permite altor taskuri să ruleze
  }
}





void taskAlternateImages(void *pvParameters) {
    int lastFunction = -1;
    static unsigned long lastSwitchTime = 0;
    const unsigned long switchInterval = 30000;

    while (true) {
        // Verifică dacă task-ul de plot update rulează
        if (xSemaphoreTake(plotUpdateSemaphore, 0) == pdTRUE) {
            xSemaphoreGive(plotUpdateSemaphore); // Eliberează semaforul imediat

            unsigned long currentMillis = millis();

            if (currentMillis - lastSwitchTime >= switchInterval) {
                lastSwitchTime = currentMillis;

                if (xSemaphoreTake(mutex, portMAX_DELAY) == pdTRUE) {
                    int selectedFunction = (lastFunction + 1) % 5;
                    lastFunction = selectedFunction;

                    switch (selectedFunction) {
                        case 0:
                            updateClockDisplay();
                            break;
                        case 1:
                            displayWiFiIP();
                            break;
                        case 2:
                            displayCurrentFrequency_ST7789();
                            break;
                        case 3:
                            initDisplay_analog();
                            for (int i = 0; i < 30; i++) {
                                struct tm timeinfo;
                                if (getLocalTime(&timeinfo)) {
                                    updateClockDisplay_analog();
                                }
                                vTaskDelay(1000 / portTICK_PERIOD_MS);
                            }
                            break;
                        case 4:
                            displayTemperature(&weatherData);
                            Serial.println("Temperature displayed.");
                            break;
                    }
                    xSemaphoreGive(mutex);
                }
            }
        }

        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}

void taskCheckFrequency(void *pvParameters) {
    static float lastFrequency = -1;

    while (true) {
        // Verifică dacă task-ul de plot update rulează
        if (xSemaphoreTake(plotUpdateSemaphore, 0) == pdTRUE) {
            xSemaphoreGive(plotUpdateSemaphore); // Eliberează semaforul imediat

            float currentFrequency = si4735.getFrequency() / 100.0;

            if (currentFrequency != lastFrequency) {
                lastFrequency = currentFrequency;

                if (xSemaphoreTake(mutex, portMAX_DELAY) == pdTRUE) {
                    displayNoiseLevel();
                    xSemaphoreGive(mutex);
                }
            }
        }

        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}





// Task pentru RDS și afișarea timpului UTC
void taskRds(void *pvParameters)
{
  // Această funcție va rula într-un task separat care se ocupă cu RDS și actualizarea informațiilor la fiecare 10 secunde
  unsigned long previousMillis = 0; // Timpul la care a fost ultima dată executat codul
  const long interval = 10000;      // Intervalul de 10 secunde (în milisecunde)

  while (true)
  {
    unsigned long currentMillis = millis(); // Obținem timpul curent

    // Verificăm dacă au trecut 10 secunde
    if (currentMillis - previousMillis >= interval)
    {
      previousMillis = currentMillis; // Actualizăm timpul de referință

      // Apelează funcțiile care gestionează RDS și afișează timpul UTC
      rds(); // Afișează informațiile RDS
      displayUtcTime();  // Afișează timpul UTC
      //rds_all_info(); // Afișează toate informațiile RDS
      //getCurrentReceivedSignalQuality(); // Afișează calitatea semnalului
    }

    delay(10); // Pauză scurtă pentru a preveni blocarea taskului
  }
}


// Task pentru afișarea timpului NTP
void taskNTPTimeDisplay(void *pvParameters)
{
  while (true)
  {
    // Generăm un număr aleatoriu între 0 și 1
    int randomChoice = random(0, 2); // 0 pentru timp, 1 pentru frecvență

    if (randomChoice == 0)
    {
      TM1638_displayNTPTime(); // Afișează timpul pe display-ul TM1638
    }
    else
    {
      displayCurrentFrequency(); // Afișează frecvența curentă pe display-ul TM1638
    }

    // Așteaptă între 1 și 60 de secunde înainte de următoarea execuție
    int delayInterval = random(10000, 60000); // Între 1 și 60 secunde
    vTaskDelay(delayInterval / portTICK_PERIOD_MS);
  }
}

// Task to control TM1638
void taskTM1638Control(void *pvParameters)
{
  while (true)
  {

    TM1638_handleVolumeUp();   // Apelează funcția pentru a gestiona butoanele de volum
    TM1638_handleVolumeDown(); // Apelează funcția pentru a gestiona butoanele de volum

    TM1638_handleFrequencyUp();   // Apelează funcția pentru a gestiona butoanele de frecvență
    TM1638_handleFrequencyDown(); // Apelează funcția pentru a gestiona butoanele de frecvență

    TM1638_CSV_handleFrequencyUp();   // Apelează funcția pentru a gestiona butoanele de frecvență
    TM1638_CSV_handleFrequencyDown(); // Apelează funcția pentru a gestiona butoanele de frecvență

    TM1638_handleTestButton(); // Apelează funcția pentru a gestiona butonul de luminozitate

    TM1638_mute(); // Apelează funcția pentru a gestiona mute/unmute

    delay(100); // Pauză pentru a permite altor taskuri să ruleze
  }
}

// Task pentru actualizarea datelor meteo la fiecare 30 de minute
void taskFetchWeatherData(void *pvParameters) {
    while (true) {
        fetchWeatherData(&weatherData);  // Preia datele meteo
        Serial.println("Weather data updated.");
        vTaskDelay(1800000 / portTICK_PERIOD_MS);  // Așteaptă 30 de minute
    }
}








void taskUpdatePlot(void *pvParameters) {
    while (true) {
        // Așteaptă semaforul pentru a începe actualizarea plotului
        if (xSemaphoreTake(plotUpdateSemaphore, portMAX_DELAY) == pdTRUE) {
            while (isPlotUpdateRunning) {
                // Pune mutex-ul pentru a bloca accesul la resurse partajate
                if (xSemaphoreTake(mutex, portMAX_DELAY) == pdTRUE) {
                    updatePlot();  // Actualizează și desenează graficul
                    //displayCpuLoadText(cpuLoad);
                    xSemaphoreGive(mutex);  // Eliberează mutex-ul
                }

                // Așteaptă 50 de milisecunde între actualizări
                vTaskDelay(50 / portTICK_PERIOD_MS);
            }

            // Eliberează semaforul când task-ul se oprește
            xSemaphoreGive(plotUpdateSemaphore);
        }

        // Așteaptă 100 ms pentru a evita blocarea
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}



void taskMeniu(void *pvParameters) {
    while (true) {
        // Verifică dacă monitorul serial rulează (flag-ul este setat)
        bool running = false;
        if (xSemaphoreTake(mutex, portMAX_DELAY) == pdTRUE) {
            running = isSerialMonitorRunning;
            xSemaphoreGive(mutex);
        }

        if (running && Serial.available() > 0) {
            char input = Serial.read(); // Citim caracterul de la utilizator

            // Ignorăm caracterele de control '\r' și '\n'
            if (input == '\r' || input == '\n') {
                continue;
            }

            // Pune mutex-ul pentru a bloca accesul la resurse partajate
            if (xSemaphoreTake(mutex, portMAX_DELAY) == pdTRUE) {
                switch (nivelCurent) {
                    case MENIU_PRINCIPAL:
                        handleMeniuPrincipal(input);
                        break;
                    case SUBMENIU_SERIAL:
                        handleSubmeniuSerial(input);
                        break;
                    case SUBMENIU_FREQUENCY:
                        handleSubmeniuFrequency(input);
                        break;
                    case SUBMENIU_FM_BUCURESTI:
                        handleSubmeniuPosturiFM(input);
                        break;
                }
                xSemaphoreGive(mutex);  // Eliberează mutex-ul
            }
        }

        vTaskDelay(10 / portTICK_PERIOD_MS);  // Pauză scurtă pentru a preveni blocarea task-ului
    }
}






/*
// Task-ul pentru verificarea frecvenței
void taskCheckFrequency(void *pvParameters) {
    static float lastFrequency = -1; // Ultima frecvență cunoscută

    while (true) {
        float currentFrequency = si4735.getCurrentRSSI(); // Obține valoarea RSSI

        if (currentFrequency != lastFrequency) {
            lastFrequency = currentFrequency;

            // Pune mutex-ul pentru a bloca task-ul de alternare
            if (xSemaphoreTake(mutex, portMAX_DELAY) == pdTRUE) {
                grafic_rssi(); // Afișează graficul RSSI
                xSemaphoreGive(mutex); // Eliberează mutex-ul
            }
        }

        vTaskDelay(100 / portTICK_PERIOD_MS); // Verifică la fiecare 100ms
    }
}
*/


void loop()
{
  // Nu facem nimic în loop
}