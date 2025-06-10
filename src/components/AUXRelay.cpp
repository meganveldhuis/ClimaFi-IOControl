#include "AUXRelay.h"

AUXRelay::AUXRelay(uint8_t relayNum, String relayName, String thermostatID, float setPoint, bool isCool, bool isPump, bool isReverse){
    this->relayNum = relayNum;
    this->relayName = relayName;
    this->thermostatID = thermostatID;
    this->setPoint = setPoint;
    this->isCool = isCool;
    this->isPump = isPump;
    this->isReverse = isReverse;

    if(isReverse){
        this->_pin = REV_IN_PIN;
    }else{
        switch(relayNum){
            case 1:
                this->_pin = AUX1_IN_PIN;
                break;
            case 2:
                this->_pin = AUX2_IN_PIN;
                break;
            case 3:
                this->_pin = AUX3_IN_PIN;
                break;
            case 4:
                this->_pin = AUX4_IN_PIN;
                break;
            default:
                this->_pin = 0;
                break;
        };
    }
    
    if(_pin != 0){
        pinMode(_pin, OUTPUT);
        digitalWrite(_pin, LOW);
    }

    isOpen = false;
}

void AUXRelay::checkTemp(String thermostatID, float currentTemp){

    if(this->thermostatID != thermostatID){
        return;
    }
    Serial.printf("Set Point for %s : %.2f. Current temp detected: %.2f. Cooling? %d\n",relayName.c_str(), setPoint, currentTemp, isCool);
    if(_pin == 0){
        Serial.println("No pin found. Aborting.");
        return;
    }
    if(setPoint > currentTemp){ // Too Cold
        if(isCool){
            if(isOpen){ //only close an opened port
                close();
                return;
            }
        }else{
            if(!isOpen){ // only open a closed port
                open();
                return;
            }
        }
    }else{ // Too Hot
        if(isCool){
            if(!isOpen){
                open();
                return;
            }
        }else{
            if(isOpen){
                close();
                return;
            }
        }
    }
    return; //otherwise nothing changed
}

bool AUXRelay::isThermostatIDRelevant(String thermostatID){
    return this->thermostatID == thermostatID; //returns true if relevant, and false otherwise
}

void AUXRelay::open(){
    if(!isOpen){
        Serial.printf("Opening pin: %d, which is in %s\n", _pin, relayName.c_str());
        digitalWrite(_pin, HIGH);
        this->isOpen = true;
        return;
    }
    return;
}

void AUXRelay::close(){
    if(isOpen){
        Serial.printf("Closing pin: %d, which is in %s\n", _pin, relayName.c_str());
        digitalWrite(_pin, LOW);
        isOpen = false;
        return;
    }
    return;
}
