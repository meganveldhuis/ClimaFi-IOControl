#include "IOControl.h"

void formatLittleFS() {
  Serial.println("Formatting LittleFS...");
  LittleFS.format();
  Serial.println("LittleFS formatted.");
}

JsonDocument readData(fs::FS &fs, const char * path){
    JsonDocument doc;
    // Open file for reading
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

    int controller = doc["controller"];
    JsonObject controllerTypes = doc["controllerTypes"];
    JsonArray components = doc["components"];

    return doc;
}

void controlLoop(){
    return;
}

void createControllerClasses(JsonDocument doc){
    String controller;
    std::vector<zoneOutputs> zoneOutputsList;

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
                for (JsonObject dataItem : data) {
                    zoneOutputs zone(
                        dataItem["zoneID"].as<int>(),
                        dataItem["zoneName"].as<String>(), 
                        dataItem["thermostatID"].as<int>(), 
                        dataItem["setPoint"].as<double>(), 
                        dataItem["isCool"].as<bool>(),
                        false //is Pump = false
                    );
                    zoneOutputsList.push_back(zone);
                }

                Serial.println("Zone Outputs List:");
                for (auto &zone : zoneOutputsList) {
                    zone.show();
                    zone.checkThermostatID(7, 12);
                }
            } else if(component["settingType"] == "Thermostat"){
                // Thermostat will be handled elsewhere
            }
        }
    } else if (controller == "ZonePumpController"){
        // handle ZonePumpController
    } else if (controller == "FanCoilController"){
        // handle FanCoilController
    } else {
        Serial.println("Could not determine controller type");
    }
}

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