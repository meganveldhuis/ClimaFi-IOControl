#include "zoneOutputs.h"

zoneOutputs::zoneOutputs(String zoneName, int thermostatID, double setPoint, bool isCool, bool isPump){
    this->zoneName = zoneName;
    this->thermostatID = thermostatID;
    this->setPoint = setPoint;
    this->isCool = isCool;
    this->isPump = isPump;
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

void zoneOutputs::checkThermostatID(int ID, double currentTemp){
    if(ID == thermostatID){
        Serial.println("correct thermostatID");
    }
}

void zoneOutputs::openValve(){

}

void zoneOutputs::closeValve(){

}