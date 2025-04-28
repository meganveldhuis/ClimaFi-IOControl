#ifndef IOCONTROL_H
#define IOCONTROL_H

/*
  IOControl.h - Library for Input/Output Control for climate systems.
  Created by Megan Veldhuis, April 2, 2025.
*/

#include "LittleFS.h"
#include "ArduinoJson.h"
#include <vector>
#include <map>

#include "components/zoneOutput.h"
#include "components/endSwitch.h"
#include "components/ADCOutput.h"
#include "components/thermistorPort.h"
#include "components/thermostat.h"

extern std::vector<zoneOutput> zoneOutputsList;
extern std::vector<thermistorPort> thermistorPortsList;
extern std::vector<thermostat> thermostatList;

extern std::map<String, bool> thermostatStates;

extern endSwitch globalThermostatEndSwitch;
extern endSwitch globalZoneEndSwitch;
extern ADCOutput globalADCOutput;
extern String globalControllerType;

void controlSetup();
JsonDocument readData(fs::FS &fs, const char * path);
void initZoneOutputs(JsonArray data, bool isPump);
void createControllerClasses(JsonDocument doc);
void formatLittleFS();
void tempUpdated(String thermostatID, float currentTemp);
bool updateSetPoint(float newSetPoint, String zoneID, String fanCoilName);


void updateControls();
bool isZoneOpen(String zoneID);
bool isADCOn();
void stateChanged(String thermostatID, bool isThermostatOn);
float getThermistorTemp(String thermistorName);
bool isThermostatOn(String thermostatID);
bool areAllThermostatsOff();
int getPortStatus();

#endif