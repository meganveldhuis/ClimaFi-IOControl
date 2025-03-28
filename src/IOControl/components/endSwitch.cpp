#include "endSwitch.h"

endSwitch::endSwitch(bool isNearZone, bool isNearThermostat, int nearID){
    Serial.println("End switch initializing");
    this->isNearZone = isNearZone;
    this->isNearThermostat = isNearThermostat;
    this->nearID = nearID;
    _pin = ZCR_OUT_PIN;
    Serial.println("---");

    pinMode(_pin, OUTPUT);
    digitalWrite(_pin, LOW);

    _isOpen = false;
    Serial.println("End switch initialized");
    Serial.println(nearID);
}

void endSwitch::open(){
    if(!_isOpen){
        Serial.printf("Opening end switch");
        // digitalWrite(_pin, LOW);
        _isOpen = true;
    }
    return;
}

void endSwitch::close(){
    if(_isOpen){
        Serial.printf("Closing end switch");
        // digitalWrite(_pin, LOW);
        _isOpen = false;
    }
    return;
}