#ifndef IOCONTROL_H
#define IOCONTROL_H

#include "LittleFS.h"
#include "ArduinoJson.h"
#include <vector>

#include "components/zoneOutputs.h"

void controlSetup();
void controlLoop();
void readData(fs::FS &fs, const char * path);
void createControllerClasses();
void formatLittleFS();

// #defineJsonDocument doc;



#endif