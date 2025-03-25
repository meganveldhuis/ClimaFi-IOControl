#ifndef ZONEOUTPUTS_H
#define ZONEOUTPUTS_H

#include "ArduinoJson.h"
#include <iostream>

class zoneOutputs{
    public:
        int zoneID;
        String zoneName;
        int thermostatID;
        double setPoint;
        bool isCool;
        bool isPump;
        int pin;

        zoneOutputs(int id, String zoneName, int thermostatID, double setPoint, bool isCool, bool isPump);
        void show();
        void checkThermostatID(int thermostatID, double currentTemp);
        void openValve();
        void closeValve();
};


#endif