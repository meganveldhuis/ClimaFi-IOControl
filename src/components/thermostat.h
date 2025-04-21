#ifndef THERMOSTAT_H
#define THERMOSTAT_H

#include "ArduinoJson.h"
#include "pin_defines.h"

class thermostat{
    public:
        int id;
        String name;
        int type;
        
        thermostat::thermostat(int id, String name, int type);
        bool isOn();
    private:
        int _pin;
};


#endif