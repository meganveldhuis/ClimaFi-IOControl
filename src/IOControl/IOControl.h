#ifndef IOCONTROL_H
#define IOCONTROL_H

#include "LittleFS.h"
// #include "nlohmann/json.hpp"
#include "ArduinoJson.h"

void controlSetup();
void controlLoop();
void readData(fs::FS &fs, const char * path);
// void writeData(String data); 
// void deleteData();

#endif