#include "ADCOutput.h";

ADCOutput::ADCOutput(String name, int thermostatID, double setPoint){
    this->name = name;
    this->thermostatID = thermostatID;
    this->setPoint = setPoint;
    this->_pin = COIL_PWM_PIN;
    pinMode(_pin, OUTPUT);
    digitalWrite(_pin, LOW);
    isOn = false;
}

void ADCOutput::turnOff(){
    if(isOn){
        digitalWrite(_pin, LOW);
        isOn = false;
    }
    return;
}

void ADCOutput::turnOn(){
    if(!isOn){
        digitalWrite(_pin, HIGH);
        isOn = true;
    }
    return;
}