#include "IOControl.h"
// Define and initialize global components
std::vector<zoneOutput> zoneOutputsList;
std::vector<thermistorPort> thermistorPortsList;
ADCOutput globalADCOutput("", -1,-1);
endSwitch globalEndSwitch(false, false, -1, false);

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
            false
        );
        zoneOutputsList.push_back(zone);
    }
}

void initEndSwitch(JsonArray data, bool isPump){
    globalEndSwitch = endSwitch (
        data[0]["isNearZone"].as<bool>(), 
        data[0]["isNearThermostat"].as<bool>(), 
        data[0]["nearID"].as<int>(),
        isPump
    );
}

void createControllerClasses(JsonDocument doc){
    String controller;

    JsonObject controllerTypes = doc["controllerTypes"];
    JsonArray components = doc["components"];

    for (JsonPair kv : controllerTypes) {
        if(doc["controller"] == kv.value().as<int>()){
            controller = kv.key().c_str();
        }
    }
    if(controller == "ZoneValveController"){
        for (JsonObject component : components){
            JsonArray data = component["data"];
            if(component["settingType"] == "controlledZoneOutputs"){
                initZoneOutputs(data, false);
            }
            if(component["settingType"] == "endSwitch"){
                initEndSwitch(data, false);
            }
        }
    } else if (controller == "ZonePumpController"){
        for (JsonObject component : components){
            JsonArray data = component["data"];
            if(component["settingType"] == "controlledZoneOutputs"){
                initZoneOutputs(data, true);
            }
            if(component["settingType"] == "endSwitch"){
                initEndSwitch(data, true);
            }
        }
    } else if (controller == "FanCoilController"){
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
            }
            if(component["settingType"] == "ADCOutput"){
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

bool tempUpdated(int thermostatID, float currentTemp){
    bool isChanged = false;
    for (zoneOutput& zone : zoneOutputsList) {
        isChanged = isChanged || zone.checkTemp(thermostatID, currentTemp);
        // Serial.println("check global end switch:");
        // Serial.println(isChanged);
        // Serial.println(globalEndSwitch.isNearZone);
        // Serial.println(globalEndSwitch.nearID);
        // Serial.println(zone.zoneID);
        if(isChanged && globalEndSwitch.isNearZone && globalEndSwitch.nearID == zone.zoneID){
            if(zone.isOpen){
                globalEndSwitch.open();
            }else{
                globalEndSwitch.close();
            }
        }
    }
    if(globalADCOutput.name){
        globalADCOutput.checkTemp(thermostatID, currentTemp);
    }
    return isChanged;
}

bool updateSetPoint(float newSetPoint, int zoneID, String fanCoilName){
    // ! this assumes you inputted a zoneID or fanCoilName that exists

    // update settings.json - but this will have to rewrite the entire file
    JsonDocument backupDocument = readData(LittleFS, "/settings.json");
    JsonDocument changedDocument = readData(LittleFS, "/settings.json");

    JsonArray components = changedDocument["components"];
    for (JsonObject component : components){
        JsonArray data = component["data"];
        for (JsonObject dataItem : data) {
            if(dataItem["zoneID"] == zoneID){
                dataItem["setPoint"] = newSetPoint;
            }else if(dataItem["name"] != "" && dataItem["name"] == fanCoilName){
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
    std::vector<zoneOutput> zoneOutputsList;
    endSwitch globalEndSwitch(false, false, -1, false);
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