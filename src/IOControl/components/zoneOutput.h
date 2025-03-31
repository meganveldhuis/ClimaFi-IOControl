#ifndef ZONEOUTPUT_H
#define ZONEOUTPUT_H

#include "ArduinoJson.h"
#include "LittleFS.h"
#include <iostream>
#include "pin_defines.h"

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
        void printData();
        bool checkTemp(int thermostatID, float currentTemp);
        void open();
        void close();

    private:
        int _pin;
        
        
};


#endif