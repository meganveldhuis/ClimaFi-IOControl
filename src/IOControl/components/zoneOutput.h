#ifndef ZONEOUTPUT_H
#define ZONEOUTPUT_H

#include "ArduinoJson.h"
#include "LittleFS.h"
#include <iostream>

// #define ZN1_OUT_PIN 35;
// #define ZN2_OUT_PIN 34;
#define ZN1_OUT_PIN 25;
#define ZN2_OUT_PIN 26;
#define ZN3_OUT_PIN 33;
#define ZN4_OUT_PIN 32;

// #define ZCR_OUT_PIN 34;
#define ZN1_IN_PIN 33;
#define ZN2_IN_PIN 32;
#define ZN3_IN_PIN 31;

class zoneOutput{
    public:
        int zoneID;
        String zoneName;
        int thermostatID;
        double setPoint;
        bool isCool;
        bool isPump;
        
        zoneOutput(int id, String zoneName, int thermostatID, double setPoint, bool isCool, bool isPump);
        void printData();
        bool checkTemp(int thermostatID, double currentTemp);
        bool open();
        bool close();
        
    private:
        int _pin;
        bool _isOpen;
};


#endif