#include "zoneOutput.h"

zoneOutput::zoneOutput(int zoneID, String zoneName, int thermostatID, float setPoint, bool isCool, bool isPump){
    this->zoneID = zoneID;
    this->zoneName = zoneName;
    this->thermostatID = thermostatID;
    this->setPoint = setPoint;
    this->isCool = isCool;
    this->isPump = isPump;
    if(isPump){
        switch(zoneID){
            case 1:
                this->_pin = ZN1_IN_PIN;
                break;
            case 2:
                this->_pin = ZN2_IN_PIN;
                break;
            case 3:
                this->_pin = ZN3_IN_PIN;
                break;
            default:
                this->_pin = 0;
                break;
        };
    }else{
        switch(zoneID){
            case 1:
                this->_pin = ZN1_OUT_PIN;
                break;
            case 2:
                this->_pin = ZN2_OUT_PIN;
                break;
            case 3:
                this->_pin = ZN3_OUT_PIN;
                break;
            case 4:
                this->_pin = ZN4_OUT_PIN;
                break;
            default:
                this->_pin = 0; //indicate error
                break;
        };
    }
    if(_pin != 0){
        pinMode(_pin, OUTPUT);
        digitalWrite(_pin, LOW);
    }

    isOpen = false;
}

bool zoneOutput::checkTemp(int thermostatID, float currentTemp){
    bool isRequestingHeat = false;
    if(this->thermostatID != thermostatID){
        return false;
    }
    Serial.printf("Set Point for %s : %.2f. Current temp detected: %.2f. Cooling? %d\n",zoneName.c_str(), setPoint, currentTemp, isCool);
    if(_pin == 0){
        Serial.println("No pin found. Aborting.");
        return false;
    }
    if(setPoint > currentTemp){ // Too Cold
        isRequestingHeat = true;
        if(isCool){
            close();
        }else{
            open();
        }
    }else{ // Too Hot
        isRequestingHeat = false;
        if(isCool){
            open();
        }else{
            close();
        }
    }
    return isRequestingHeat;
    
}

bool zoneOutput::isThermostatIDRelevant(int thermostatID){
    return this->thermostatID == thermostatID; //returns true if relevant, and false otherwise
}

void zoneOutput::open(){
    if(!isOpen){
        Serial.printf("Opening pin: %d, which is in %s\n", _pin, zoneName.c_str());
        digitalWrite(_pin, HIGH);
        this->isOpen = true;
        return;
    }
    return;
}

void zoneOutput::close(){
    if(isOpen){
        Serial.printf("Closing pin: %d, which is in %s\n", _pin, zoneName.c_str());
        digitalWrite(_pin, LOW);
        isOpen = false;
        return;
    }
    return;
}
