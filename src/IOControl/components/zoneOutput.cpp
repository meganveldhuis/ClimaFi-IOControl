#include "zoneOutput.h"

zoneOutput::zoneOutput(int zoneID, String zoneName, int thermostatID, double setPoint, bool isCool, bool isPump){
    this->zoneID = zoneID;
    this->zoneName = zoneName;
    this->thermostatID = thermostatID;
    this->setPoint = setPoint;
    this->isCool = isCool;
    this->isPump = isPump;
    if(isPump){
        switch(zoneID){ // CHANGE ACCORDING TO PUMP CONTROLLER HARDWARE
            case 0:
                this->_pin = 0;
            case 1:
                this->_pin = 34;
            case 2:
                this->_pin = 33;
            case 3:
                this->_pin = 32;
            default:
                this->_pin = 0;
        };
    }else{
        switch(zoneID){
            case 0:
                this->_pin = 0;
            case 1:
                this->_pin = ZN1_OUT_PIN;
            case 2:
                this->_pin = ZN2_OUT_PIN;
            case 3:
                this->_pin = ZN3_OUT_PIN;
            case 4:
                this->_pin = ZN4_OUT_PIN;
            default:
                this->_pin = 0;
        };
    }
    pinMode(_pin, OUTPUT);
    digitalWrite(_pin, LOW);

    _isOpen = false;
    
}


void zoneOutput::show(){
    Serial.print("Zone Name:");
    Serial.println(zoneName);
    Serial.print("Thermostat ID: ");
    Serial.println(thermostatID);
    Serial.print("Set Point: ");
    Serial.println(setPoint);
    Serial.print("Is Cool: ");
    Serial.println(isCool);
}

void zoneOutput::checkTemp(int thermostatID, double currentTemp){
    if(this->thermostatID != thermostatID){
        return;
    }
    if(setPoint > currentTemp){ // too cold
        if(isCool){
            close();
        }else{
            open();
        }
    }else{ // too hot
        if(isCool){
            open();
        }else{
            close();
        }
    }
    
}

void zoneOutput::open(){
    if(!_isOpen){
        digitalWrite(_pin, HIGH);
        _isOpen = true;
    }
}

void zoneOutput::close(){
    if(_isOpen){
        digitalWrite(_pin, LOW);
        _isOpen = false;
    }
}

void zoneOutput::updateSetPoint(double newSetPoint, JsonDocument doc){
    this->setPoint = newSetPoint;
    // update settings.json - but this will have to rewrite the entire file
}