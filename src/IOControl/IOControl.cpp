#include "IOControl.h"
std::vector<zoneOutput> zoneOutputsList;
endSwitch globalEndSwitch(false, false, -1); // Define and initialize global endSwitch

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

    // Deserialize the JSON document
    DeserializationError error = deserializeJson(doc, file);
    
    // Close the file
    file.close();

    // Test if parsing succeeds
    if (error) {
        Serial.println("deserializeJson() failed: ");
        Serial.println(error.f_str());
        return doc;
    }

    return doc;
}

void controlLoop(){
    return;
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
    Serial.println("read data:");
    Serial.println(data[0]["isNearZone"].as<bool>());
    Serial.println("read data:");
    Serial.println(data[0]["nearID"].as<int>());
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
        Serial.println("pump found");
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
        // handle FanCoilController
    } else {
        Serial.println("Could not determine controller type");
    }
}



// ------------------ Library functions to use in other files ------------------ //

void controlSetup(){
    Serial.begin(115200);
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
    // ! this assumes you put in a zoneID that exists

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
    serializeJsonPretty(changedDocument, Serial);
    
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