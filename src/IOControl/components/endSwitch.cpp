#include "endSwitch.h"

endSwitch::endSwitch(bool isNearZone, bool isNearThermostat, int nearID){
    this->isNearZone = isNearZone;
    this->isNearThermostat = isNearThermostat;
    this->nearID = nearID;
    _pin = ZCR_OUT_PIN;

    pinMode(_pin, OUTPUT);
    digitalWrite(_pin, LOW);

    _isOpen = false;
}

void endSwitch::open(){
    if(!_isOpen){
        Serial.printf("Opening end switch\n");
        // digitalWrite(_pin, LOW);
        _isOpen = true;
    }
    return;
}

void endSwitch::close(){
    if(_isOpen){
        Serial.printf("Closing end switch\n");
        // digitalWrite(_pin, LOW);
        _isOpen = false;
    }
    return;
}