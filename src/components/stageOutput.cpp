#include "stageOutput.h"

stageOutput::stageOutput(uint8_t stageNum, String stageName, String thermostatID, uint8_t rank, float setPoint, bool isCool, bool isPump){
    this->stageName = stageName;
    this->thermostatID = thermostatID;
    this->setPoint = setPoint;
    this->isCool = isCool;
    this->rank = rank;
    this->isPump = isPump;

    switch(stageNum){
        case 1:
            this->_pin = STG1_IN_PIN;
            break;
        case 2:
            this->_pin = STG2_IN_PIN;
            break;
        case 3:
            this->_pin = STG3_IN_PIN;
            break;
        case 4:
            this->_pin = STG4_IN_PIN;
            break;
        default:
            this->_pin = 0; //indicate error
            break;
    };

    if(_pin != 0){
        pinMode(_pin, OUTPUT);
        digitalWrite(_pin, LOW);
    }

    this->isOpen = false;
}

uint8_t stageOutput::checkTemp(String thermostatID, float currentTemp){
    //RETURN VALUE :
        // 0 = closed port
        // 1 = opened port
        // 9 = nothing changed
    if(this->thermostatID != thermostatID){
        return NO_CHANGE;
    }
    Serial.printf("Set Point for %s : %.2f. Current temp detected: %.2f. Cooling? %d Open? %d\n",stageName.c_str(), setPoint, currentTemp, isCool, this->isOpen);
    if(_pin == 0){
        Serial.println("No pin found. Aborting.");
        return NO_CHANGE;
    }
    if(setPoint > currentTemp){ // Too Cold
        if(isCool){
            if(this->isOpen){ //only close an opened port
                close();
                return CLOSED_PORT;
            }
        }else{
            if(!this->isOpen){ // only open a closed port
                open();
                return OPENED_PORT;
            }
        }
    }else{ // Too Hot
        if(isCool){
            if(!this->isOpen){
                open();
                return OPENED_PORT;
            }
        }else{
            if(this->isOpen){
                close();
                return CLOSED_PORT;
            }
        }
    }
    return NO_CHANGE; //otherwise nothing changed
}

bool stageOutput::isThermostatIDRelevant(String thermostatID){
    return this->thermostatID == thermostatID; //returns true if relevant, and false otherwise
}

void stageOutput::open(){
    if (!this->isOpen) {
        Serial.printf("Opening pin: %d, which is in %s. Is open: %d\n", _pin, stageName.c_str(), this->isOpen);
        digitalWrite(_pin, HIGH);
        this->isOpen = true;
        Serial.printf("Pin %d is now open. Is open: %d\n", _pin, this->isOpen);
    }
    return;
}

void stageOutput::close(){
    if (this->isOpen) {
        Serial.printf("Closing pin: %d, which is in %s. Is open: %d\n", _pin, stageName.c_str(), this->isOpen);
        digitalWrite(_pin, LOW);
        this->isOpen = false;
        Serial.printf("Pin %d is now closed. Is open: %d\n", _pin, this->isOpen);
    }
    return;
}
