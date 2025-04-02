#ifndef ENDSWITCH_H
#define ENDSWITCH_H

#include <Arduino.h>
#include "pin_defines.h"

class endSwitch{
    public:
        bool isZone;

        endSwitch(bool isZone, bool isPump);
        void open();
        void close();
    private:
        int _pin;
        bool _isOpen;

};

#endif