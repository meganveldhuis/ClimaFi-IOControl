#ifndef THERMOSTAT_H
#define THERMOSTAT_H

#include "ArduinoJson.h"
#include "pin_defines.h"

class thermostat{
    public:
        String id;
        String name;
        uint8_t type;
        
        thermostat(uint8_t thermostatNum, String id, String name, uint8_t type);
        bool isOn();
    private:
        uint8_t _pin;
};


#endif