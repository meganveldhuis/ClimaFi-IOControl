#include "IOControl.h"

// // Define and initialize global components
// std::vector<zoneOutput> zoneOutputsList;
// std::vector<stageOutput> stageOutputsList;
// std::vector<AUXRelay> AUXRelaysList;
// std::vector<thermistorPort*> thermistorPortsList; //need to use a pointer in order for Polymorphism to work (and have an ADCThermistor object inside the vector)
// std::vector<thermostat> thermostatList;

// std::map<String, bool> thermostatStates;

// ADCOutput globalADCOutput("", "", 1, 0, "");
// endSwitch globalThermostatEndSwitch(true, false);
// endSwitch globalZoneEndSwitch(false, false);
// String globalControllerType = "";

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


void initZoneOutputs(JsonArray data){
    for (JsonObject dataItem : data) {
        zoneOutput zone(
            dataItem["zoneNum"].as<uint8_t>(), //this is the "zone number" as displayed on the PCB
            dataItem["zoneID"].as<String>(),
            dataItem["zoneName"].as<String>(), 
            dataItem["thermostatID"].as<String>(), 
            dataItem["rank"].as<uint8_t>(),
            dataItem["setPoint"].as<float>(), 
            dataItem["isCool"].as<bool>(),
            dataItem["isPump"].as<bool>(),
            globalControllerType
        );
        zoneOutputsList.push_back(zone);
    }
}

void initStageOutputs(JsonArray data){
    for (JsonObject dataItem : data) {
        stageOutput stage(
            dataItem["stageNum"].as<uint8_t>(),
            dataItem["stageName"].as<String>(),
            dataItem["thermostatID"].as<String>(),
            dataItem["rank"].as<uint8_t>(),
            dataItem["setPoint"].as<float>(),
            dataItem["isCool"].as<bool>(),
            dataItem["isPump"].as<bool>()
        );
        stageOutputsList.push_back(stage);
    }
}

void initAuxRelays(JsonArray data){
    for (JsonObject dataItem : data) {
        AUXRelay relay(
            dataItem["relayNum"].as<uint8_t>(),
            dataItem["relayName"].as<String>(),
            dataItem["thermostatID"].as<String>(),
            dataItem["rank"].as<uint8_t>(),
            dataItem["setPoint"].as<float>(), 
            dataItem["isCool"].as<bool>(),
            dataItem["isPump"].as<bool>(),
            dataItem["isReverse"].as<bool>()
        );
        AUXRelaysList.push_back(relay);
    }
}

void initThermostats(JsonArray data){
    for (JsonObject dataItem : data) {
        thermostat thermostatInstance(
            dataItem["thermostatNum"].as<uint8_t>(),
            dataItem["thermostatID"].as<String>(),
            dataItem["name"].as<String>(),
            dataItem["type"].as<uint8_t>(),
            globalControllerType
        );
        thermostatList.push_back(thermostatInstance);
    }
}

bool createControllerClasses(JsonDocument doc){

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
                initZoneOutputs(data);
            } else if(component["componentType"] == "zoneEndSwitch"){
                globalZoneEndSwitch = endSwitch(true, false);
            // } else if(component["componentType"] == "thermostatEndSwitch"){
            //     globalThermostatEndSwitch = endSwitch(false, false);
            } else if(component["componentType"] == "thermostat"){
                JsonArray data = component["data"];
                initThermostats(data);
            }
        }
        return true;
    } else if (globalControllerType == "ZonePumpController"){
        Serial.println("Pump detected");
        for (JsonObject component : components){
            JsonArray data = component["data"];
            if(component["componentType"] == "controlledZoneOutputs"){
                initZoneOutputs(data);
            } else if(component["componentType"] == "zoneEndSwitch"){
                globalZoneEndSwitch = endSwitch(true, true);
            } else if(component["componentType"] == "thermostatEndSwitch"){
                globalThermostatEndSwitch = endSwitch(false, true);
            } else if(component["componentType"] == "thermostat"){
                initThermostats(data);
            }
        }
        return true;
    } else if (globalControllerType == "FanCoilController"){
        Serial.println("Fan coil detected");
        for (JsonObject component : components){
            JsonArray data = component["data"];
            if(component["componentType"] == "thermistorPort"){
                int number = 1;
                for (JsonObject dataItem : data) {
                    thermistorPortsList.push_back(new thermistorPort(dataItem["name"].as<String>(),
                        number,
                        dataItem["id"].as<uint8_t>())
                    );
                    number++;
                }
            } else if(component["componentType"] == "ADCOutput"){
                globalADCOutput = ADCOutput(
                    data[0]["name"].as<String>(),
                    data[0]["thermostatID"].as<String>(),
                    data[0]["rank"].as<uint8_t>(),
                    data[0]["setPoint"].as<float>(),
                    globalControllerType
                );
            }
        }
        return true;
    } else if (globalControllerType == "HeatPumpController"){
        Serial.println("Heat Pump Controller detected");
        for (JsonObject component : components){
            JsonArray data = component["data"];
            if(component["componentType"] == "relay"){ // Aux relay (x 4) + Rev Valve relay (x 1)
                initAuxRelays(data);
            } else if(component["componentType"] == "stagingOutputs"){ // Stage Outputs / Heat pumps/valves ( x 4)
                initStageOutputs(data);
            } else if(component["componentType"] == "ADCOutput"){ // Fan coil
                globalADCOutput = ADCOutput(
                    data[0]["name"].as<String>(),
                    data[0]["thermostatID"].as<String>(),
                    data[0]["rank"].as<uint8_t>(),
                    data[0]["setPoint"].as<float>(),
                    globalControllerType
                );
            } else if(component["componentType"] == "demandSignals"){ // thermostat demands
                initThermostats(data);
            } else if(component["componentType"] == "thermistor"){ // thermistors (x 8)
                // TwoWire I2CBus = TwoWire(0);
                // Adafruit_ADS1115 ads;
                // I2CBus.begin(17, 18);
                // if(!ads.begin(0x48, &I2CBus)){
                //     Serial.println("Failed to initialize ADS.");
                // }
                uint8_t number = 1;
                for (JsonObject dataItem : data) {
                    thermistorPortsList.push_back(new ADCThermistor(
                        dataItem["name"].as<String>(),
                        number,
                        dataItem["id"].as<uint8_t>())
                    );
                    number++;
                }
            }
        }
        return true;
    } else {
        Serial.println("Could not determine controller type");
        return false;
    }
}



// ------------------ Library functions to use in other files ------------------ //

bool controlSetup(){
    /* 
        @brief Setup the control components based on the settings.json file
        @return If successful, will return true. if there was an error (opening the settings.json file or detecting the controller type), will return false.
    */
    JsonDocument doc;
    if (!LittleFS.begin()) {
        Serial.println("An Error has occurred while mounting LittleFS. Formatting and trying again");
        formatLittleFS();
        if (!LittleFS.begin()) {
            Serial.println("Failed to mount LittleFS after formatting");
            return false;
        }
    }
    doc = readData(LittleFS, "/settings.json");
    bool created = createControllerClasses(doc);
    return created;
}

void updateControls(){
    /* 
        @brief Update the saved control components from the settings.json file
    */
    //reset all globals
    for (auto thermistor : thermistorPortsList) {
        delete thermistor;
    }
    thermistorPortsList.clear();

    zoneOutputsList.clear();
    stageOutputsList.clear();
    AUXRelaysList.clear();
    thermostatList.clear();
    thermostatStates.clear();

    ADCOutput globalADCOutput("", "", 1, 0, "");
    endSwitch globalThermostatEndSwitch(true, false);
    endSwitch globalZoneEndSwitch(false, false);
    String globalControllerType = "";

    // Call setup again
    controlSetup();
    return;
}

void tempUpdated(String thermostatID, float currentTemp){
    /* 
        @brief Call when a thermostat has a new temperature measured. This function will, for each zoneOutput, check if the thermostat ID is relevant and will open/close based on the saved set point. 
        @param thermostatID The ID of the thermostat
        @param currentTemp The measured temperature
    */
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
    /* 
        @brief To be called when a wired thermostat state has changed. This will open/close any zone output that has the relevant thermostatID, or open/close the fan coil if it has the relevant thermostatID
        @param thermostatID The ID of the thermostat whose state was changed
        @param isThermostatOn A boolean representing the (changed) state of the thermostat
    */
    // call when the wired thermostat states have changed
    if(globalControllerType == "FanCoilController"){
        if(globalADCOutput.isThermostatIDRelevant(thermostatID)){ // check if thermostat ID is relevant
            if(isThermostatOn){
                globalADCOutput.turnOn();
            }else{
                globalADCOutput.turnOff();
            }
        }
    } else if(globalControllerType == "HeatPumpController"){
        for (stageOutput& stage : stageOutputsList) {
            if(stage.isThermostatIDRelevant(thermostatID)){ //check if thermostatID is relevant
                if(isThermostatOn){
                    stage.open();
                }else{
                    stage.close();
                }
            }
        }
    } else{ // zone valve or pump controller:
        for (zoneOutput& zone : zoneOutputsList) {
            if(zone.isThermostatIDRelevant(thermostatID)){ //check if thermostatID is relevant
                if(isThermostatOn){
                    zone.open();
                }else{
                    zone.close();
                }
            }
        }
    }
    return;
}



bool updateSetPoint(float newSetPoint, String zoneID, String fanCoilName){
    /* 
        @brief Update the set point of a component
        @param newSetPoint The desired set point to update to
        @param zoneID The ID of the zone outputs that a change is requested (valves/pumps)
        @param fanCoilName The name of the Fan Coil that a change is requested (optional)
        @return A boolean if the function worked and if set point was successfully updated
    */

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

bool isZoneOpen(String zoneID){
    /* 
        @brief Check status of the zone output (valve or pump)
        @param zoneID The ID of the zone
        @return The status (true if open, false if closed) of the valve/pump
    */
    for (zoneOutput& zone : zoneOutputsList) {
        if(zone.zoneID == zoneID){
            return zone.isOpen;
        }
    }
    Serial.printf("No zone with id %d found\n", zoneID);
    return false;
}

bool isADCOn(){
    /* 
        @brief Checks if the ADC Output (Fan Coil) is on.
        @return boolean if the ADC Output is on (true if on, false if off)
    */
    return globalADCOutput.isOn;
}

bool areAllThermostatsOff() {
    /* 
        @brief Check if all thermostats are off. 
        @return true if all thermostats are off, and false if any (at least one) are on
    */
    for (auto it = thermostatStates.begin(); it != thermostatStates.end(); ++it) {
        if (it->second) { // Check the value (isOn)
            return false; // At least one thermostat is on
        }
    }
    return true; // All thermostats are off
}

float getThermistorTemp(String thermistorName){
    /* 
        @brief Get the temperature from an inputted thermistor
        @param thermistorName The name of the thermistor
        @return The temperature read from the given thermistor
    */
    for (auto thermistor : thermistorPortsList) {
        if(thermistor->name == thermistorName){
            return thermistor->getTemp();
        }
    }
    Serial.printf("No thermistor with name %s found\n", thermistorName.c_str());
    return 0;
}

uint8_t getThermistorIDByName(String thermistorName){
    /* 
        @brief Get the thermistor's ID by its name
        @param thermistorName The name of the thermistor
        @return The thermistor ID of the given thermistor
    */
    for (auto thermistor : thermistorPortsList) {
        if(thermistor->name == thermistorName){
            return thermistor->id;
        }
    }
    Serial.printf("No thermistor with name %s found\n", thermistorName.c_str());
    return 0;
}

bool isThermostatOn(String thermostatID){
    /* 
        @brief Check status of thermostat
        @param thermostatID The thermostatID of the desired thermostat
        @return the status (bool) of the wired thermostat - true is On, false is Off
    */
    // check state of (wired) thermostat : is it on or off
    for (thermostat& thermostat : thermostatList) {
        if (thermostat.id == thermostatID) {
            return thermostat.isOn();
        }
    }
    Serial.printf("No thermostat with id %d found\n", thermostatID);
    return false;
}

uint16_t getPortStatus(){
    /*
        @brief Get the status of output ports. If it's a FanCoilController, it will return the status of the fan coil. If it is a ZoneValveController or ZonePumpController, this will return the status of the valves or pumps as a buffer
        @return If FanCoilController, returns the ADCOutput status. Else, returns the status for all the Zone Outputs as a buffer
    */
    uint16_t status = 0b0;
    if(globalControllerType == "FanCoilController"){
        status = globalADCOutput.isOn ? 0b1 : 0b0;
        return status;
    } else if(globalControllerType == "HeatPumpController"){
        uint16_t bitPosition = 0; // Track the bit position for each zone
        for (stageOutput& stage : stageOutputsList) {
            bool thisStatus = stage.isOpen;
            if (thisStatus) {
                status |= (1 << bitPosition); // Set the corresponding bit to 1
            }
            bitPosition++; // Move to the next bit position
        }
    } else{
        uint16_t bitPosition = 0; // Track the bit position for each zone
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