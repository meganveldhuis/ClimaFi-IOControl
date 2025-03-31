#ifndef THERMISTORPORT_H
#define THERMISTORPORT_H

#include <Arduino.h>
#include "pin_defines.h"

class thermistorPort{
    public:
        String name;
    
        thermistorPort(String name, int number);
        float getTemp();

    private:
        int _pin;

};

#endif