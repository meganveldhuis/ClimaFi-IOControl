#ifndef ENDSWITCH_H
#define ENDSWITCH_H

#include <Arduino.h>
#include "pin_defines.h"

class endSwitch{
    public:
        bool isZone;

        endSwitch(bool isZone, bool isPump);
        bool open();
        bool close();
    private:
        uint8_t _pin;
        bool _isOpen;

};

#endif