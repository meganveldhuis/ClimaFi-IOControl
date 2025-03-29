#include <Arduino.h>
#include "IOControl/IOControl.h"

void setup(){

  Serial.begin(115200);
  controlSetup();
  tempUpdated(7, 29);
  updateSetPoint(9.75, 2);
  // Serial.println(ESP.getFreeHeap());

  Serial.println("Setup complete");
}

void loop(){
  controlLoop();

  delay(5);
}