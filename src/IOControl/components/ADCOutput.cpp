#include "ADCOutput.h"

ADCOutput::ADCOutput(String name, int thermostatID, double setPoint){
    this->name = name;
    this->thermostatID = thermostatID;
    this->setPoint = setPoint;
    this->_pin = COIL_PWM_PIN;
    pinMode(_pin, OUTPUT);
    analogWrite(_pin, LOW);
    isOn = false;
}

void ADCOutput::checkTemp(int thermostatID, double currentTemp){
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