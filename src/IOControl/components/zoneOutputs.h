#ifndef ZONEOUTPUTS_H
#define ZONEOUTPUTS_H

#include "ArduinoJson.h"
#include <iostream>

class zoneOutputs{
    public:
        String zoneName;
        int thermostatID;
        double setPoint;
        bool isCool;
        bool isPump;

        zoneOutputs(String zoneName, int thermostatID, double setPoint, bool isCool, bool isPump);
        void show();
        void checkThermostatID(int ID, double currentTemp);
        void openValve();
        void closeValve();
};


#endif