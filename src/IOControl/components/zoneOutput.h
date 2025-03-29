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
        double setPoint;
        bool isCool;
        bool isPump;
        bool isOpen;
        
        
        zoneOutput(int id, String zoneName, int thermostatID, double setPoint, bool isCool, bool isPump);
        void printData();
        bool checkTemp(int thermostatID, double currentTemp);
        void open();
        void close();

    private:
        int _pin;
        
        
};


#endif