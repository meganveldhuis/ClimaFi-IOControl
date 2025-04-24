#include "endSwitch.h"

endSwitch::endSwitch(bool isZone, bool isPump){
    this->isZone = isZone;
    if(!isPump){ // valve controller
        _pin = ISO_END_PIN;
    }else if (isPump && isZone){ // zone switch on pump controller
        _pin = ZCR_OUT_PIN;
    }else if(isPump && !isZone){ // thermostat switch on pump controller
        _pin = ISO_IN_PIN;
    }else{
        _pin = 0; // error
        Serial.println("ERROR::: Code bug. Failed to assign pin");
    }
    
    pinMode(_pin, OUTPUT);
    digitalWrite(_pin, LOW);

    _isOpen = false;
}

void endSwitch::open(){
    if(!_isOpen){
        digitalWrite(_pin, HIGH);
        _isOpen = true;
        Serial.printf("opened zone end switch on pin %d\n", _pin);
    }
    return;
}

void endSwitch::close(){
    if(_isOpen){
        digitalWrite(_pin, LOW);
        _isOpen = false;
        Serial.printf("closed zone end switch on pin %d\n", _pin);
    }
    return;
}