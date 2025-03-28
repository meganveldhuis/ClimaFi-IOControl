#include <Arduino.h>

#include "IOControl/IOControl.h"

void setup(){

  Serial.begin(115200);
  Serial.println("before setup");
  controlSetup();
  tempUpdated(7, 29);
  // updateSetPoint(9.75, 2);
  Serial.println(ESP.getFreeHeap());

  Serial.println("Setup complete");
}

void loop()
{
    //nc_loop();
    controlLoop();

    delay(5);
}