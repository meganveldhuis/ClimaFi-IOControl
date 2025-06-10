#ifndef ADCOUTPUT_H
#define ADCOUTPUT_H

#include <Arduino.h>
#include "pin_defines.h"

class ADCOutput{
    public:
        String name;
        String thermostatID;
        uint8_t rank;
        uint16_t setPoint;
        bool isOn;
    
        ADCOutput(String name, String thermostatID, uint8_t rank, float setPoint, String controllerType);
        void checkTemp(String thermostatID, float currentTemp);
        bool isThermostatIDRelevant(String thermostatID);
        void turnOn();
        void turnOff();
        
    private:
        uint8_t _pin;

};

#endif