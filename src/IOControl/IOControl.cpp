#include "IOControl.h"
#include <vector>

JsonDocument doc; // Allocate JsonDocument globally

class controlledZoneOutputs {
    public:
        String zoneName;
        int thermostatID;
        double setPoint;
        bool isCool;

        controlledZoneOutputs(String zoneName, int thermostatID, double setPoint, bool isCool){
            this->zoneName = zoneName;
            this->thermostatID = thermostatID;
            this->setPoint = setPoint;
            this->isCool = isCool;
        }

        void show(){
            Serial.printf("Zone Name: %s\n", zoneName);
            Serial.printf("Thermostat ID: %d\n", thermostatID);
            Serial.printf("Set Point: %d\n", setPoint);
            Serial.printf("Is Cool: %s\n", isCool);
        }
};

class thermostat {
    public:
        String name;
        String type;
};

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
    // int controllerInt = ;
    String controller;
    std::vector<controlledZoneOutputs> zoneOutputsList;

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
                    String zoneName = dataItem["zoneName"].as<String>();
                    int thermostatID = dataItem["thermostatID"].as<int>();
                    double setPoint = dataItem["setPoint"].as<double>();
                    bool isCool = dataItem["isCool"].as<bool>();
                    
                    controlledZoneOutputs zone(zoneName, thermostatID, setPoint, isCool);
                    zoneOutputsList.push_back(zone);
                }

                Serial.println("Zone Outputs List:");
                for (auto &zone : zoneOutputsList) {
                    zone.show();
                }
            } else if(component["settingType"] == "Thermostat"){
                // ...handle Thermostat...
            }
        }
    } else if (controller == "ZonePumpController"){
        // ...handle ZonePumpController...
    } else if (controller == "FanCoilController"){
        // ...handle FanCoilController...
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