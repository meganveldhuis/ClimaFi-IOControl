#ifndef ENDSWITCH_H
#define ENDSWITCH_H

#include <Arduino.h>
#include "pin_defines.h"

class endSwitch{
    public:
        bool isNearZone;
        bool isNearThermostat;
        int nearID;

        endSwitch(bool isNearZone, bool isNearThermostat, int nearID, bool isPump);
        void open();
        void close();
    private:
        int _pin;
        bool _isOpen;

};

#endif