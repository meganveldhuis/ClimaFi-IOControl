#include "IOControl.h"

// Define and initialize global components
std::vector<zoneOutput> zoneOutputsList;
std::vector<thermistorPort> thermistorPortsList;
std::vector<thermostat> thermostatList;

std::unordered_map<int, bool> thermostatStates;

ADCOutput globalADCOutput("", -1,-1);
endSwitch globalThermostatEndSwitch(true, false);
endSwitch globalZoneEndSwitch(false, false);
String globalControllerType = "";

void formatLittleFS() {
  Serial.println("Formatting LittleFS...");
  LittleFS.format();
  Serial.println("LittleFS formatted.");
}

JsonDocument readData(fs::FS &fs, const char * path){
    JsonDocument doc;
    File file = fs.open(path, "r");
    if (!file) {
        Serial.println("Failed to open file for reading");
        return doc;
    }
    DeserializationError error = deserializeJson(doc, file);
    file.close();
    
    if (error) { // Test if parsing succeeds
        Serial.println("deserializeJson() failed: ");
        Serial.println(error.f_str());
        return doc;
    }
    serializeJsonPretty(doc, Serial);
    return doc;
}


void initZoneOutputs(JsonArray data, bool isPump){
    for (JsonObject dataItem : data) {
        zoneOutput zone(
            dataItem["zoneID"].as<int>(),
            dataItem["zoneName"].as<String>(), 
            dataItem["thermostatID"].as<int>(), 
            dataItem["setPoint"].as<float>(), 
            dataItem["isCool"].as<bool>(),
            isPump
        );
        zoneOutputsList.push_back(zone);
    }
}

void initThermostats(JsonArray data){
    for (JsonObject dataItem : data) {
        thermostat thermostatInstance(
            dataItem["thermostatID"].as<int>(),
            dataItem["name"].as<String>(), 
            dataItem["type"].as<int>()
        );
        thermostatList.push_back(thermostatInstance);
    }
}

void createControllerClasses(JsonDocument doc){

    JsonObject controllerTypes = doc["controllerTypes"];
    JsonArray components = doc["components"];

    for (JsonPair kv : controllerTypes) {
        if(doc["controller"] == kv.value().as<int>()){
            globalControllerType = kv.key().c_str();
        }
    }

    if(globalControllerType == "ZoneValveController"){
        Serial.println("Valve detected");
        for (JsonObject component : components){
            if(component["componentType"] == "controlledZoneOutputs"){
                JsonArray data = component["data"];
                initZoneOutputs(data, false);
            } else if(component["componentType"] == "zoneEndSwitch"){
                globalZoneEndSwitch = endSwitch(true, false);
            } else if(component["componentType"] == "thermostatEndSwitch"){
                globalThermostatEndSwitch = endSwitch(false, false);
            } else if(component["componentType"] == "thermostat"){
                JsonArray data = component["data"];
                initThermostats(data);
            }
        }
    } else if (globalControllerType == "ZonePumpController"){
        Serial.println("Pump detected");
        for (JsonObject component : components){
            JsonArray data = component["data"];
            if(component["componentType"] == "controlledZoneOutputs"){
                initZoneOutputs(data, true);
            } else if(component["componentType"] == "zoneEndSwitch"){
                globalZoneEndSwitch = endSwitch(true, true);
            } else if(component["componentType"] == "thermostatEndSwitch"){
                globalThermostatEndSwitch = endSwitch(false, true);
            } else if(component["componentType"] == "thermostat"){
                JsonArray data = component["data"];
                initThermostats(data);
            }
        }
    } else if (globalControllerType == "FanCoilController"){
        Serial.println("Fan coil detected");
        for (JsonObject component : components){
            JsonArray data = component["data"];
            if(component["componentType"] == "thermistorPort"){
                int number = 1;
                for (JsonObject dataItem : data) {
                    thermistorPort thermistor(
                        dataItem["name"].as<String>(),
                        number
                    );
                    number++;
                    thermistorPortsList.push_back(thermistor);
                }
            } else if(component["componentType"] == "ADCOutput"){
                globalADCOutput = ADCOutput(
                    data[0]["name"].as<String>(),
                    data[0]["thermostatID"].as<int>(),
                    data[0]["setPoint"].as<float>()
                );
            }
        }
    } else {
        Serial.println("Could not determine controller type");
    }
}



// ------------------ Library functions to use in other files ------------------ //

void controlSetup(){
    JsonDocument doc;
    if (!LittleFS.begin()) {
        Serial.println("An Error has occurred while mounting LittleFS. Formatting and trying again");
        formatLittleFS();
        if (!LittleFS.begin()) {
            Serial.println("Failed to mount LittleFS after formatting");
            return;
        }
    }
    doc = readData(LittleFS, "/settings.json");
    createControllerClasses(doc);
}

void tempUpdated(int thermostatID, float currentTemp){

    if(globalControllerType == "FanCoilController"){
        globalADCOutput.checkTemp(thermostatID, currentTemp);
    }else{ // zone valve or pump controller:
        bool isAnyOpened = false;
        for (zoneOutput& zone : zoneOutputsList) {
            int response = zone.checkTemp(thermostatID, currentTemp);
            if(response == 0){ //Closed Port
                // TODO : broadcast that this zone was opened
            }else if (response == 1){ //Opened Port
                // TODO : broadcast that this zone was opened
            } //else, nothing happened, so do nothing

            if(zone.isOpen){
                isAnyOpened = true;
            }
        }

        if(isAnyOpened){
            globalZoneEndSwitch.open();
        }else{
            globalZoneEndSwitch.close();
        }
    }
    return;
}

void stateChanged(int thermostatID, bool isThermostatOn){
    // call when the wired thermostat states have changed
    if(globalControllerType == "FanCoilController"){
        if(globalADCOutput.isThermostatIDRelevant(thermostatID)){ // check if thermostat ID is relevant
            if(isThermostatOn){
                globalADCOutput.turnOn();
            }else{
                globalADCOutput.turnOff();
            }
        }
    }else{ // zone valve or pump controller:
        bool isAnyOpened = false;
        bool isRequestingHeat = false;
        for (zoneOutput& zone : zoneOutputsList) {
            if(zone.isThermostatIDRelevant(thermostatID)){ //check if thermostatID is relevant
                if(isThermostatOn){
                    zone.open();
                }else{
                    zone.close();
                }
            }
            if(zone.isOpen){
                isAnyOpened = true;
            }
        }

        if(isAnyOpened){
            globalZoneEndSwitch.open();
        }else{
            globalZoneEndSwitch.close();
        }

        if(isThermostatOn){
            globalThermostatEndSwitch.open();
        }else if(areAllThermostatsOff()){
            globalThermostatEndSwitch.close();
        }else{
            globalThermostatEndSwitch.open();
        }
    }
    return;
}

bool areAllThermostatsOff() {
    for (const auto& [thermostatID, isOn] : thermostatStates) {
        if (isOn) {
            return false; // At least one thermostat is on
        }
    }
    return true; // All thermostats are off
}

bool updateSetPoint(float newSetPoint, int zoneID, String fanCoilName){

    // update settings.json - but this will have to rewrite the entire file
    JsonDocument backupDocument = readData(LittleFS, "/settings.json");
    JsonDocument changedDocument = readData(LittleFS, "/settings.json");

    JsonArray components = changedDocument["components"];
    for (JsonObject component : components){
        JsonArray data = component["data"];
        for (JsonObject dataItem : data) {
            if(globalControllerType == "FanCoilController" && dataItem["name"] == fanCoilName){
                dataItem["setPoint"] = newSetPoint;
            }else if((globalControllerType== "ZoneValveController" || globalControllerType== "ZonePumpController") && dataItem["zoneID"] == zoneID){
                dataItem["setPoint"] = newSetPoint;
            }
        }
    }
    
    LittleFS.remove("/settings.json"); // Delete existing file, otherwise the configuration is appended to the file
    File file = LittleFS.open("/settings.json", "w");
    if (!file) {
        Serial.println("Failed to open file for writing");
        return false;
    }
    if (serializeJson(changedDocument, file) == 0) {
        Serial.println(F("Failed to write to file. Reset to old settings"));
        if (serializeJson(backupDocument, file) == 0) {
            Serial.println(F("Failed to write to backup file. Uh oh"));
            return false;
        }
        return false;
    }
    file.close();

    for (zoneOutput& zone : zoneOutputsList) {
        if(zone.zoneID == zoneID){
            zone.setPoint = newSetPoint;
        }
    }
    if(globalADCOutput.name == fanCoilName){
        globalADCOutput.setPoint = newSetPoint;
    }
    Serial.printf("Successfully changed setPoint to %.3f in Zone %u\n", newSetPoint, zoneID);
    return true;
}

void updateControls(){
    //reset globals
    std::vector<zoneOutput> zoneOutputsList;
    std::vector<thermistorPort> thermistorPortsList;
    std::vector<thermostat> thermostatList;
    std::unordered_map<int, bool> thermostatStates;
    controlSetup();
    return;
}

bool isZoneOpen(int zoneID){
    for (zoneOutput& zone : zoneOutputsList) {
        if(zone.zoneID == zoneID){
            return zone.isOpen;
        }
    }
    Serial.printf("No zone with id %d found\n", zoneID);
    return false;
}


bool isADCOn(){
    return globalADCOutput.isOn;
}

float getThermistorTemp(String thermistorName){
    for (thermistorPort& thermistor : thermistorPortsList) {
        if(thermistor.name == thermistorName){
            return thermistor.getTemp();
        }
    }
    Serial.printf("No thermistor with name %s found\n", thermistorName.c_str());
    return 0;
}

bool isThermostatOn(int thermostatID){
    // check state of (wired) thermostat : is it on or off
    for (thermostat& thermostat : thermostatList) {
        if (thermostat.id == thermostatID) {
            return thermostat.isOn();
        }
    }
    Serial.printf("No thermostat with id %d found\n", thermostatID);
    return false;
}