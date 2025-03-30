#include <Arduino.h>
#include "IOControl/IOControl.h"

void setup(){
  Serial.begin(115200);

  controlSetup();
  tempUpdated(7, 29);

  delay(4000);
  Serial.println("Updating set point");
  updateSetPoint(80, 2);
  
  delay(4000);
  Serial.println("with new file:");
  updateControls();

  Serial.println("Setup complete");
}

void loop(){
  delay(5);
}