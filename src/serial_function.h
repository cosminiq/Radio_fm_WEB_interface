#ifndef SERIAL_FUNCTION_H
#define SERIAL_FUNCTION_H

void showStatus();
void handleMuteUnmute();
void handleVolumeUp();
void handleVolumeDown();
void handleFrequencyUp();
void handleFrequencyDown();
void handleSeekStationUp();
void handleSeekStationDown();
void handleBandwidthChange();
void handleAMFMSelection(char key);
void handleFrequencyInput(); //nu fuctioneaza cum as fi vrut
void citesteFisierCSV(); 

#endif
