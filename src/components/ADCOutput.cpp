#include "ADCOutput.h"

ADCOutput::ADCOutput(String name, int thermostatID, float setPoint){
    this->name = name;
    this->thermostatID = thermostatID;
    this->setPoint = setPoint;
    this->_pin = COIL_PWM_PIN;
    pinMode(_pin, OUTPUT);
    analogWrite(_pin, LOW);
    isOn = false;
}

void ADCOutput::checkTemp(int thermostatID, float currentTemp){
    if(this->thermostatID != thermostatID){
        return;
    }
    if(setPoint > currentTemp){
        turnOn();
        
    }else{
        turnOff();
    }
    return;
}

bool ADCOutput::isThermostatIDRelevant(int thermostatID){
    return this->thermostatID == thermostatID; //returns true if relevant, and false otherwise
}

void ADCOutput::turnOn(){
    if(!isOn){
        analogWrite(_pin, HIGH);
        isOn = true;
    }
    return;
}

void ADCOutput::turnOff(){
    if(isOn){
        analogWrite(_pin, LOW);
        isOn = false;
    }
    return;
}