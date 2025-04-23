#ifndef ADCOUTPUT_H
#define ADCOUTPUT_H

#include <Arduino.h>
#include "pin_defines.h"

class ADCOutput{
    public:
        String name;
        int thermostatID;
        int rank;
        float setPoint;
        bool isOn;
    
        ADCOutput(String name, int thermostatID, int rank, float setPoint);
        void checkTemp(int thermostatID, float currentTemp);
        bool isThermostatIDRelevant(int thermostatID);
        void turnOn();
        void turnOff();
        
    private:
        int _pin;

};

#endif