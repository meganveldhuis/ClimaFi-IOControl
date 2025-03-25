#include "zoneOutputs.h"

zoneOutputs::zoneOutputs(int zoneID, String zoneName, int thermostatID, double setPoint, bool isCool, bool isPump){
    this->zoneID = zoneID;
    this->zoneName = zoneName;
    this->thermostatID = thermostatID;
    this->setPoint = setPoint;
    this->isCool = isCool;
    this->isPump = isPump;
    if(isPump){
        switch(zoneID){ // CHANGE ACCORDING TO PUMP CONTROLLER HARDWARE
            case 0:
                this->pin = 23;
            case 1:
                this->pin = 24;
            case 2:
                this->pin = 25;
            case 3:
                this->pin = 26;
            default:
                this->pin = 0;
        };
    }else{
        switch(zoneID){ // CHANGE ACCORDING TO VALVE CONTROLLER HARDWARE
            case 0:
                this->pin = 23;
            case 1:
                this->pin = 24;
            case 2:
                this->pin = 25;
            case 3:
                this->pin = 26;
            default:
                this->pin = 0;
        };
    }
    
}


void zoneOutputs::show(){
    Serial.print("Zone Name:");
    Serial.println(zoneName);
    Serial.print("Thermostat ID: ");
    Serial.println(thermostatID);
    Serial.print("Set Point: ");
    Serial.println(setPoint);
    Serial.print("Is Cool: ");
    Serial.println(isCool);
}

void zoneOutputs::checkThermostatID(int thermostatID, double currentTemp){
    if(this->thermostatID == thermostatID){
        Serial.println("correct thermostatID");
        if(setPoint > currentTemp){ // need to be hotter
            if(isCool){
                if(isPump){

                }else{ // Valve
                    closeValve();
                }
            }else{
                if(isPump){

                }else{ // Valve
                    openValve();
                }
            }
        }else{ //need to be cooler
            if(isCool){
                if(isPump){

                }else{ // Valve
                    openValve();
                }
            }else{
                if(isPump){

                }else{ // Valve
                    closeValve();
                }
            }
        }
        
    }
}

void zoneOutputs::openValve(){
    // open valvePin
}

void zoneOutputs::closeValve(){
    // close valvePin
}