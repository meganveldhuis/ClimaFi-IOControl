#ifndef THERMISTORPORT_H
#define THERMISTORPORT_H

#include <Arduino.h>
#include "pin_defines.h"
#include <Adafruit_ADS1X15.h>
#include <Wire.h>

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

class ADCThermistor: public thermistorPort {
    public:
        float getTemp();
        ADCThermistor(String name, uint8_t number, uint8_t id);
    private:
        Adafruit_ADS1115 ads;
        TwoWire I2CBus;
};

#endif