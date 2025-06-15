#ifndef GLOBALS_H
#define GLOBALS_H

#include <vector>
#include <map>
#include <Arduino.h>
#include <Adafruit_ADS1X15.h>
#include <Wire.h>

#include "pin_defines.h"
#include "components/zoneOutput.h"
#include "components/stageOutput.h"
#include "components/AUXRelay.h"
#include "components/thermostat.h"
#include "components/ADCOutput.h"
#include "components/endSwitch.h"

// Forward declarations to avoid circular dependency
class thermistorPort;

// Global variables
extern std::vector<zoneOutput> zoneOutputsList;
extern std::vector<stageOutput> stageOutputsList;
extern std::vector<AUXRelay> AUXRelaysList;
extern std::vector<thermistorPort*> thermistorPortsList; // Polymorphism
extern std::vector<thermostat> thermostatList;

extern std::map<String, bool> thermostatStates;

extern ADCOutput globalADCOutput;
extern endSwitch globalThermostatEndSwitch;
extern endSwitch globalZoneEndSwitch;
extern String globalControllerType;

extern TwoWire I2CBus;
extern Adafruit_ADS1115 ads;

#endif