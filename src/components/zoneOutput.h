#ifndef ZONEOUTPUT_H
#define ZONEOUTPUT_H

#include "ArduinoJson.h"
#include "LittleFS.h"
#include <iostream>
#include "pin_defines.h"

#define CLOSED_PORT 0
#define OPENED_PORT 1
#define NO_CHANGE 9

class zoneOutput{
    public:
        int zoneID;
        String zoneName;
        int thermostatID;
        float setPoint;
        bool isCool;
        bool isPump;
        bool isOpen;
        
        
        zoneOutput(int id, String zoneName, int thermostatID, float setPoint, bool isCool, bool isPump);
        int checkTemp(int thermostatID, float currentTemp);
        bool isThermostatIDRelevant(int thermostatID);
        void open();
        void close();

    private:
        int _pin;
        
        
};


#endif