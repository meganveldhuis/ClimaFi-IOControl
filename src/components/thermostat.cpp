#include "thermostat.h"

thermostat::thermostat(int id, String name, int type){
    this->id = id;
    this->name = name;
    this->type = type;
    switch(id){
        case 1: 
            this->_pin = ZN1_SENSE_PIN;
            break;
        case 2: 
            this->_pin = ZN2_SENSE_PIN;
            break;
        case 3: 
            this->_pin = ZN3_SENSE_PIN;
            break;
        case 4: 
            this->_pin = ZN4_SENSE_PIN;
            break;
        default:
            this->_pin = 0;
            break;
    }
    if(_pin != 0){
        pinMode(_pin, INPUT);
    }
}

bool thermostat::isOn(){
    if(_pin == 0){
        Serial.println("No pin found. Aborting.");
        return false; //error
    }
    int state = digitalRead(_pin);
    if(state == HIGH){
        return true;
    }else if(state == LOW){
        return false;
    }else{
        Serial.println("Error reading pin.");
        return false;
    }
}