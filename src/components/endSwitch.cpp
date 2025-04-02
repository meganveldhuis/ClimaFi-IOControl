#include "endSwitch.h"

endSwitch::endSwitch(bool isZone, bool isPump){
    this->isZone = isZone;
    if(isPump){
        if(isZone){
            _pin = ZCR_OUT_PIN;
        }else{
            _pin = ISO_IN_PIN;
        }
    }else{
        if(isZone){
            _pin = 0; //error
        }else{
            _pin = ISO_END_PIN;
        }
    }
    

    pinMode(_pin, OUTPUT);
    digitalWrite(_pin, LOW);

    _isOpen = false;
}

void endSwitch::open(){
    if(!_isOpen){
        digitalWrite(_pin, HIGH);
        _isOpen = true;
    }
    return;
}

void endSwitch::close(){
    if(_isOpen){
        digitalWrite(_pin, LOW);
        _isOpen = false;
    }
    return;
}