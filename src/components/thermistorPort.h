#ifndef THERMISTORPORT_H
#define THERMISTORPORT_H

#include <Arduino.h>
#include "pin_defines.h"
#include "globals.h"

class thermistorPort{
    public:
        String name;
        uint8_t id;
        uint8_t number;
    
        thermistorPort(String name, uint8_t number, uint8_t id);
        virtual float getTemp();

    private:
        uint8_t _pin;

};

#endif