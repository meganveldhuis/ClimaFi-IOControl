#ifndef ADCOUTPUT_H
#define ADCOUTPUT_H

#include <Arduino.h>
#include "pin_defines.h"

class ADCOutput{
    public:
        String name;
        int thermostatID;
        double setPoint;
        bool isOn;
    
        ADCOutput(String name, int thermostatID, double setPoint);
        void checkTemp(int thermostatID, double currentTemp);
        void turnOn();
        void turnOff();
        
    private:
        int _pin;

};

#endif