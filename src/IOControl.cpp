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
            if(component["settingType"] == "controlledZoneOutputs"){
                JsonArray data = component["data"];
                initZoneOutputs(data, false);
            } else if(component["settingType"] == "zoneEndSwitch"){
                globalZoneEndSwitch = endSwitch(true, false);
            } else if(component["settingType"] == "thermostatEndSwitch"){
                globalThermostatEndSwitch = endSwitch(false, false);
            } else if(component["settingType"] == "Thermostat"){
                JsonArray data = component["data"];
                initThermostats(data);
            }
        }
    } else if (globalControllerType == "ZonePumpController"){
        Serial.println("Pump detected");
        for (JsonObject component : components){
            JsonArray data = component["data"];
            if(component["settingType"] == "controlledZoneOutputs"){
                initZoneOutputs(data, true);
            } else if(component["settingType"] == "zoneEndSwitch"){
                globalZoneEndSwitch = endSwitch(true, true);
            } else if(component["settingType"] == "thermostatEndSwitch"){
                globalThermostatEndSwitch = endSwitch(false, true);
            } else if(component["settingType"] == "Thermostat"){
                JsonArray data = component["data"];
                initThermostats(data);
            }
        }
    } else if (globalControllerType == "FanCoilController"){
        Serial.println("Fan coil detected");
        for (JsonObject component : components){
            JsonArray data = component["data"];
            if(component["settingType"] == "thermistorPort"){
                int number = 1;
                for (JsonObject dataItem : data) {
                    thermistorPort thermistor(
                        dataItem["name"].as<String>(),
                        number
                    );
                    number++;
                    thermistorPortsList.push_back(thermistor);
                }
            } else if(component["settingType"] == "ADCOutput"){
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
    bool isAnyOpened = false;
    bool isRequestingHeat = false;
    if(globalControllerType == "FanCoilController"){
        globalADCOutput.checkTemp(thermostatID, currentTemp);
    }else{
        for (zoneOutput& zone : zoneOutputsList) {
            isRequestingHeat = isRequestingHeat ||  zone.checkTemp(thermostatID, currentTemp);
            if(zone.isOpen){
                isAnyOpened = true;
            }
        }
        if(isAnyOpened){
            globalZoneEndSwitch.open();
        }else{
            globalZoneEndSwitch.close();
        }
        if(isRequestingHeat){
            globalThermostatEndSwitch.open();
        }else{
            globalThermostatEndSwitch.close();
        }
    }
    return;
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
    return 0;
}

// check state of thermostat
// WIRED :
    // is it on or off

bool isThermostatOn(int thermostatID){
    for (thermostat& thermostat : thermostatList) {
        if (thermostat.id == thermostatID) {
            return thermostat.isOn();
        }
    }
    Serial.printf("No thermostat with id %d found\n", thermostatID);
    return false;
}