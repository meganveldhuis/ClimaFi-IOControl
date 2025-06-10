#ifndef AUXRELAY_H
#define AUXRELAY_H

#include <Arduino.h>
#include "pin_defines.h"

class AUXRelay{
    public:
        uint8_t relayNum;
        String relayName;
        String thermostatID;
        float setPoint;
        bool isCool;
        bool isPump;
        bool isReverse;

        bool isOpen;
        uint8_t rank;
        
        AUXRelay(uint8_t relayNum, String relayName, String thermostatID, uint8_t rank, float setPoint, bool isCool, bool isPump, bool isReverse);
        void checkTemp(String thermostatID, float currentTemp);
        bool isThermostatIDRelevant(String thermostatID);
        void open();
        void close();

    private:
        uint8_t _pin;

};

#endif