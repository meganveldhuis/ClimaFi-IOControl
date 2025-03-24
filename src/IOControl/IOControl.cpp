#include "IOControl.h"

JsonDocument doc; // TODO: move this to .h file

void formatLittleFS() {
  Serial.println("Formatting LittleFS...");
  LittleFS.format();
  Serial.println("LittleFS formatted.");
}

void readData(fs::FS &fs, const char * path){
    // Open file for reading
    File file = fs.open(path, "r");
    if (!file) {
        Serial.println("Failed to open file for reading");
        return;
    }

    // Deserialize the JSON document
    DeserializationError error = deserializeJson(doc, file);
    
    // Close the file
    file.close();

    // Test if parsing succeeds
    if (error) {
        Serial.println("deserializeJson() failed: ");
        Serial.println(error.f_str());
        return;
    }

    serializeJsonPretty(doc, Serial);

    int controller = doc["controller"];
    JsonObject controllerTypes = doc["controllerTypes"];
    JsonArray components = doc["components"];

    return;
}

void controlLoop(){
    return;
}

void createControllerClasses(){
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
                    Serial.println("creating class instance");
                    zoneOutputs zone(dataItem["zoneName"].as<String>(), 
                        dataItem["thermostatID"].as<int>(), 
                        dataItem["setPoint"].as<double>(), 
                        dataItem["isCool"].as<bool>(),
                        false
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
    if (!LittleFS.begin()) {
        Serial.println("An Error has occurred while mounting LittleFS. Formatting and trying again");
        formatLittleFS();
        if (!LittleFS.begin()) {
            Serial.println("Failed to mount LittleFS after formatting");
            return;
        }
    }
    readData(LittleFS, "/settings.json");
    createControllerClasses();
}