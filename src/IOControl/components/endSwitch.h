#ifndef ENDSWITCH_H
#define ENDSWITCH_H

#include <Arduino.h>

#define ZCR_OUT_PIN 25;

class endSwitch{
    public:
        bool isNearZone;
        bool isNearThermostat;
        int nearID;

        endSwitch(bool isNearZone, bool isNearThermostat, int nearID);
        void open();
        void close();
    private:
        int _pin;
        bool _isOpen;

};

#endif