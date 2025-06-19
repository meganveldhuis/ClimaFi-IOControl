#include "globals.h"

// Define and initialize global components
std::vector<zoneOutput*> zoneOutputsList;
std::vector<stageOutput*> stageOutputsList;
std::vector<AUXRelay*> AUXRelaysList;
std::vector<thermistorPort*> thermistorPortsList; //need to use a pointer in order for Polymorphism to work (and have an ADCThermistor object inside the vector)
std::vector<thermostat*> thermostatList;

std::map<String, bool> thermostatStates;

ADCOutput globalADCOutput("", "", 1, 0, "");
endSwitch globalThermostatEndSwitch(true, false);
endSwitch globalZoneEndSwitch(false, false);
String globalControllerType = "";