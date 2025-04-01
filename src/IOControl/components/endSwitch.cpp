#include "endSwitch.h"

endSwitch::endSwitch(bool isNearZone, bool isNearThermostat, int nearID, bool isPump){
    this->isNearZone = isNearZone;
    this->isNearThermostat = isNearThermostat;
    this->nearID = nearID;
    if(isPump){
        _pin = ZCR_OUT_PIN;
    }else{
        _pin = ISO_END_PIN;
    }

    pinMode(_pin, OUTPUT);
    digitalWrite(_pin, LOW);

    _isOpen = false;
}

void endSwitch::open(){
    if(!_isOpen){
        Serial.printf("Opening end switch\n");
        digitalWrite(_pin, HIGH);
        _isOpen = true;
    }
    return;
}

void endSwitch::close(){
    if(_isOpen){
        Serial.printf("Closing end switch\n");
        digitalWrite(_pin, LOW);
        _isOpen = false;
    }
    return;
}