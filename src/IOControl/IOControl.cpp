#include "IOControl.h"
std::vector<zoneOutput> zoneOutputsList;

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

    serializeJsonPretty(doc, Serial);
    return doc;
}

void controlLoop(){
    // tempUpdated(4, 25); //should printData that the topFloor's valve is closed
    // tempUpdated(4, 15); // should be on / cooling
    return;
}

void createZoneOutputsList(JsonArray data, bool isPump){
    for (JsonObject dataItem : data) {
        zoneOutput zone(
            dataItem["zoneID"].as<int>(),
            dataItem["zoneName"].as<String>(), 
            dataItem["thermostatID"].as<int>(), 
            dataItem["setPoint"].as<double>(), 
            dataItem["isCool"].as<bool>(),
            false //set isPump = false
        );
        zoneOutputsList.push_back(zone);
    }
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
                createZoneOutputsList(data, false);
            }
        }
    } else if (controller == "ZonePumpController"){
        for (JsonObject component : components){
            JsonArray data = component["data"];
            if(component["settingType"] == "controlledZoneOutputs"){
                createZoneOutputsList(data, true);
            }
        }
    } else if (controller == "FanCoilController"){
        // handle FanCoilController
    } else {
        Serial.println("Could not determine controller type");
    }
}



// ------ Library functions to use in other files ------ //

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
    for (zoneOutput& zone : zoneOutputsList) { // Pass by reference
        isChanged = isChanged || zone.checkTemp(thermostatID, currentTemp);
    }
    return isChanged;
}