#include "IOControl.h"

// Define and initialize global components
std::vector<zoneOutput> zoneOutputsList;
std::vector<thermistorPort> thermistorPortsList;
std::vector<thermostat> thermostatList;

std::map<String, bool> thermostatStates;

ADCOutput globalADCOutput("", "", 1, 0);
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
            dataItem["zoneNum"].as<uint8_t>(), //this is the "zone number" as displayed on the PCB
            dataItem["zoneID"].as<String>(),
            dataItem["zoneName"].as<String>(), 
            dataItem["thermostatID"].as<String>(), 
            dataItem["rank"].as<uint8_t>(),
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
            dataItem["thermostatNum"].as<uint8_t>(),
            dataItem["thermostatID"].as<String>(),
            dataItem["name"].as<String>(), 
            dataItem["type"].as<uint8_t>()
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
            // } else if(component["componentType"] == "thermostatEndSwitch"){
            //     globalThermostatEndSwitch = endSwitch(false, false);
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
                    data[0]["thermostatID"].as<String>(),
                    data[0]["rank"].as<uint8_t>(),
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

void tempUpdated(String thermostatID, float currentTemp){

    if(globalControllerType == "FanCoilController"){
        globalADCOutput.checkTemp(thermostatID, currentTemp);
    }else{ // zone valve or pump controller:
        for (zoneOutput& zone : zoneOutputsList) {
            int response = zone.checkTemp(thermostatID, currentTemp);
        }
        // turn on zone end switch outside of this function
    }
    return;
}

void stateChanged(String thermostatID, bool isThermostatOn){
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
        for (zoneOutput& zone : zoneOutputsList) {
            if(zone.isThermostatIDRelevant(thermostatID)){ //check if thermostatID is relevant
                if(isThermostatOn){
                    Serial.printf("Open noticed\n");
                    zone.open();
                }else{
                    zone.close();
                }
            }
        }
    }
    return;
}

bool areAllThermostatsOff() {
    for (auto it = thermostatStates.begin(); it != thermostatStates.end(); ++it) {
        if (it->second) { // Check the value (isOn)
            return false; // At least one thermostat is on
        }
    }
    return true; // All thermostats are off
}

bool updateSetPoint(float newSetPoint, String zoneID, String fanCoilName){

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
    std::map<String, bool> thermostatStates;
    controlSetup();
    return;
}

bool isZoneOpen(String zoneID){
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

bool isThermostatOn(String thermostatID){
    // check state of (wired) thermostat : is it on or off
    for (thermostat& thermostat : thermostatList) {
        if (thermostat.id == thermostatID) {
            return thermostat.isOn();
        }
    }
    Serial.printf("No thermostat with id %d found\n", thermostatID);
    return false;
}

int getPortStatus(){
    int status = 0b0;
    if(globalControllerType == "FanCoilController"){
        status = globalADCOutput.isOn ? 0b1 : 0b0;
        return status;
    }else{
        int bitPosition = 0; // Track the bit position for each zone
        for (zoneOutput& zone : zoneOutputsList) {
            bool thisStatus = zone.isOpen;
            if (thisStatus) {
                status |= (1 << bitPosition); // Set the corresponding bit to 1
            }
            bitPosition++; // Move to the next bit position
        }
    }
    return status;
}