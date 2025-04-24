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
        String zoneID;
        String zoneName;
        String thermostatID;
        int16_t setPoint;
        bool isCool;
        bool isPump;
        bool isOpen;
        uint8_t rank;
        
        zoneOutput(uint8_t zoneNum, String zoneID, String zoneName, String thermostatID, uint8_t rank, float setPoint, bool isCool, bool isPump);
        int checkTemp(String thermostatID, float currentTemp);
        bool isThermostatIDRelevant(String thermostatID);
        void open();
        void close();

    private:
        int _pin;
        
        
};


#endif