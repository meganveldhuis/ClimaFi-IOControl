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
#include "components/stageOutput.h"
#include "components/endSwitch.h"
#include "components/ADCOutput.h"
#include "components/thermistorPort.h"
#include "components/thermostat.h"
#include "components/AUXRelay.h"

extern std::vector<zoneOutput> zoneOutputsList;
extern std::vector<stageOutput> stageOutputsList;
extern std::vector<AUXRelay> AUXRelaysList;
extern std::vector<thermistorPort*> thermistorPortsList;
extern std::vector<thermostat> thermostatList;

extern std::map<String, bool> thermostatStates;

extern endSwitch globalThermostatEndSwitch;
extern endSwitch globalZoneEndSwitch;
extern ADCOutput globalADCOutput;
extern String globalControllerType;

void formatLittleFS();

JsonDocument readData(fs::FS &fs, const char * path);


void initZoneOutputs(JsonArray data);
void initStageOutputs(JsonArray data);
void initAuxRelays(JsonArray data);
void initThermostats(JsonArray data);
bool createControllerClasses(JsonDocument doc);

bool controlSetup();
void updateControls();

void tempUpdated(String thermostatID, float currentTemp);
void stateChanged(String thermostatID, bool isThermostatOn);


bool updateSetPoint(float newSetPoint, String zoneID, String fanCoilName="");

bool isZoneOpen(String zoneID);
bool isADCOn();

bool areAllThermostatsOff();
float getThermistorTemp(String thermistorName);
uint8_t getThermistorIDByName(String thermistorName);
bool isThermostatOn(String thermostatID);

uint16_t getPortStatus();

#endif