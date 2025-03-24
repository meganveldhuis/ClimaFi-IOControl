#include "IOControl.h"
// using json = nlohmann::json;


// run `pio run --target uploadfs` to upload new files to ESP32

void formatLittleFS() {
  Serial.println("Formatting LittleFS...");
  LittleFS.format();
  Serial.println("LittleFS formatted.");
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

}


void readData(fs::FS &fs, const char * path){
    // StaticJsonBuffer is deprecated, use DynamicJsonDocument instead
    JsonDocument doc;

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

    // Test if parsing succeeds.
    if (error) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
        return;
    }

    // The filter: it contains "true" for each value we want to keep
    // JsonDocument filter;
    // filter["list"][0]["dt"] = true;
    // filter["list"][0]["main"]["temp"] = true;

    // Fetch values.
    // Most of the time, you can rely on the implicit casts.
    // In other case, you can do doc["time"].as<long>();
    // const char* sensor = doc["sensor"];
    // long time = doc["time"];
    // double latitude = doc["data"][0];
    // double longitude = doc["data"][1];

    serializeJsonPretty(doc, Serial);


    int controller = doc["controller"];


    Serial.print("Controller: ");
    Serial.println(controller);


    JsonObject controllerTypes = doc["controllerTypes"];
    JsonArray components = doc["components"];

    Serial.println("Controller Types:");
    for (JsonPair kv : controllerTypes) {
        Serial.print("  ");
        Serial.print(kv.key().c_str());
        Serial.print(": ");
        Serial.println(kv.value().as<int>());
    }

    Serial.println("Components:");
    for (JsonObject component : components) {
        Serial.print("  Setting Type: ");
        Serial.println(component["settingType"].as<const char*>());
        JsonArray data = component["data"];
        for (JsonObject dataItem : data) {
            for (JsonPair kv : dataItem) {
                Serial.print("    ");
                Serial.print(kv.key().c_str());
                Serial.print(": ");
                Serial.println(kv.value().as<const char*>()); // Assuming all values are strings for simplicity
            }
        }
    }


}

void controlLoop(){
    return;
}

// IMPORT settings as settings.json file

// check "controller" value vs controllerTypes



// I would suggest a file load system then when it loads the JSON, it
// creates class objects that have each hardware component of the board individually.

// IF CONTROLLER IS ZoneValveController (0):
    // HARDWARE 1 : controlledZoneOutputs
        // settings:
            //  "zoneName": "livingRoomZoneName",
            //   "thermostatID": 1,
            //   "setPoint": 23,
            //   "isCool": true
        // functions:
            // tempCheck
                // if temp > setPoint :


