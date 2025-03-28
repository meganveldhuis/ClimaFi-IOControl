#ifndef IOCONTROL_H
#define IOCONTROL_H

#include "LittleFS.h"
#include "ArduinoJson.h"
#include <vector>

#include "components/zoneOutput.h"
#include "components/endSwitch.h"

extern std::vector<zoneOutput> zoneOutputsList;
extern endSwitch globalEndSwitch;

void controlSetup();
void controlLoop();
JsonDocument readData(fs::FS &fs, const char * path);
void initZoneOutputs(JsonArray data, bool isPump);
void createControllerClasses(JsonDocument doc);
void formatLittleFS();
bool tempUpdated(int thermostatID, double currentTemp);
bool updateSetPoint(double newSetPoint, int zoneID);
void initEndSwitch(JsonArray data);

void openEndSwitch();
void closeEndSwitch();

#endif