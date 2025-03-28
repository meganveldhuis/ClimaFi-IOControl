#include <Arduino.h>

#include "IOControl/IOControl.h"

void setup(){

  Serial.begin(115200);

  controlSetup();
  updateSetPoint(9.75, 2);

  Serial.println("Setup complete");
}

void loop()
{
    //nc_loop();
    controlLoop();

    delay(5);
}