#include <Arduino.h>
#include "IOControl/IOControl.h"

void setup(){
  Serial.begin(115200);

  controlSetup();
  tempUpdated(1, 10);

  delay(4000);
  Serial.println("Updating set point");
  updateSetPoint(75, 1, "fanCoilName");

  tempUpdated(1, 29);
  
  delay(4000);
  Serial.println("with new file:");
  updateControls();

  tempUpdated(1, 80);

  Serial.println(getThermistorTemp("secondThermistorName"));

  Serial.println("Setup complete");
}

void loop(){
  delay(5);
}