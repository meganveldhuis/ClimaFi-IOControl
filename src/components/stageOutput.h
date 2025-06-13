#ifndef STAGINGOUTPUT_H
#define STAGINGOUTPUT_H

#include "ArduinoJson.h"
#include "LittleFS.h"
#include <iostream>
#include "pin_defines.h"

#define CLOSED_PORT 0
#define OPENED_PORT 1
#define NO_CHANGE 9

class stageOutput{
    public:
        String stageName;
        String thermostatID;
        float setPoint;
        bool isCool;
        bool isPump;
        bool isOpen;
        uint8_t rank;

        stageOutput(uint8_t stageNum, String stageName, String thermostatID, uint8_t rank, float setPoint, bool isCool, bool isPump);
        uint8_t checkTemp(String thermostatID, float currentTemp);
        bool isThermostatIDRelevant(String thermostatID);
        void open();
        void close();

    private:
        uint8_t _pin;

};


#endif