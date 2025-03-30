#include "IOControl.h"
// Define and initialize global components
std::vector<zoneOutput> zoneOutputsList;
std::vector<thermistorPort> thermistorPortsList;
ADCOutput globalADCOutput("", -1,-1);
endSwitch globalEndSwitch(false, false, -1);

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
            dataItem["setPoint"].as<double>(), 
            dataItem["isCool"].as<bool>(),
            false
        );
        zoneOutputsList.push_back(zone);
    }
}

void initEndSwitch(JsonArray data){
    globalEndSwitch = endSwitch (
        data[0]["isNearZone"].as<bool>(), 
        data[0]["isNearThermostat"].as<bool>(), 
        data[0]["nearID"].as<int>()
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
        }
    } else if (controller == "ZonePumpController"){
        for (JsonObject component : components){
            JsonArray data = component["data"];
            if(component["settingType"] == "controlledZoneOutputs"){
                initZoneOutputs(data, true);
            }
            if(component["settingType"] == "endSwitch"){
                initEndSwitch(data);
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
                    data[0]["setPoint"].as<double>()
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

bool tempUpdated(int thermostatID, double currentTemp){
    bool isChanged = false;
    for (zoneOutput& zone : zoneOutputsList) {
        isChanged = isChanged || zone.checkTemp(thermostatID, currentTemp);
        if(isChanged && globalEndSwitch.isNearZone && globalEndSwitch.nearID == zone.zoneID){
            if(zone.isOpen){
                globalEndSwitch.open();
            }else{
                globalEndSwitch.close();
            }
        }
    }
    return isChanged;
}

bool updateSetPoint(double newSetPoint, int zoneID){
    // ! this assumes you inputted a zoneID that exists

    // update settings.json - but this will have to rewrite the entire file
    JsonDocument backupDocument = readData(LittleFS, "/settings.json");
    JsonDocument changedDocument = readData(LittleFS, "/settings.json");

    JsonArray components = changedDocument["components"];
    for (JsonObject component : components){
        JsonArray data = component["data"];
        for (JsonObject dataItem : data) {
            if(dataItem["zoneID"] == zoneID){
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
    Serial.printf("Successfully changed setPoint to %.3f in Zone %u\n", newSetPoint, zoneID);
    return true;
}

void updateControls(){
    std::vector<zoneOutput> zoneOutputsList;
    endSwitch globalEndSwitch(false, false, -1);
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