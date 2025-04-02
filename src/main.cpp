#include <Arduino.h>
#include "IOControl/IOControl.h"

void setup(){
  Serial.begin(115200);
  while (!Serial) ;
  delay(1000);
  Serial.println("Starting setup");
  controlSetup();
  // tempUpdated(1, 10);

  // delay(4000);
  // Serial.println("Updating set point");
  // updateSetPoint(75, 1, "fanCoilName");

  // tempUpdated(1, 29);
  
  // delay(4000);
  // Serial.println("with new file:");
  // updateControls();

  // tempUpdated(1, 80);

  // Serial.println(getThermistorTemp("secondThermistorName"));

  Serial.println("Setup complete");
}

void testValves(){
  // ---- TEST : tempUpdated and turn on/off valves ---- //
  // tempUpdated(1, 40);
  // tempUpdated(2, 40);
  // tempUpdated(3, 40);
  // tempUpdated(4, 40);
  // delay(5000);
  // tempUpdated(1, 10);
  // tempUpdated(2, 10);
  // tempUpdated(3, 10);
  // tempUpdated(4, 10);
  // delay(5000);
  // tempUpdated(1, 40);
  // tempUpdated(2, 40);
  // tempUpdated(3, 40);
  // tempUpdated(4, 40);
  // delay(5000);
  // tempUpdated(1, 10);
  // tempUpdated(2, 10);
  // tempUpdated(3, 10);
  // tempUpdated(4, 10);
  // delay(5000);

  // ---- TEST : Update Set Point ---- //
  // Serial.println("Updating set point");
  // updateSetPoint(25, 2, "");
  // delay(5000);
  // tempUpdated(2, 50); // should be off
  // delay(5000);
  // tempUpdated(2, 10); //should be on
  // delay(15000);

  // ---- TEST isZoneOpen() ---- //
  // tempUpdated(1, 40);
  // tempUpdated(4, 40);
  // Serial.println(isZoneOpen(1));
  // Serial.println(isZoneOpen(4));
  // delay(5000);
  // tempUpdated(1, 10);
  // tempUpdated(4, 10);
  // Serial.println(isZoneOpen(1));
  // Serial.println(isZoneOpen(4));
  // delay(5000);
}

void testPumps(){
  // ---- TEST : tempUpdated and turn on/off valves ---- //
  // tempUpdated(1, 40);
  // tempUpdated(2, 40);
  // tempUpdated(3, 40);
  // delay(5000);
  // tempUpdated(1, 10);
  // tempUpdated(2, 10);
  // tempUpdated(3, 10);
  // delay(5000);
  // tempUpdated(1, 40);
  // tempUpdated(2, 40);
  // tempUpdated(3, 40);
  // delay(5000);
  // tempUpdated(1, 10);
  // tempUpdated(2, 10);
  // tempUpdated(3, 10);
  // delay(5000);

  // ---- TEST : Update Set Point ---- //
  // Serial.println("Updating set point");
  // updateSetPoint(70, 2, "");
  // delay(5000);
  // tempUpdated(2, 80); // should be off
  // delay(5000);
  // tempUpdated(2, 25); //should be on
  // delay(15000);

  // ---- TEST isZoneOpen() ---- //
  // tempUpdated(1, 40);
  // tempUpdated(2, 40);
  // Serial.println(isZoneOpen(1));
  // Serial.println(isZoneOpen(2));
  // delay(5000);
  // tempUpdated(1, 10);
  // tempUpdated(2, 10);
  // Serial.println(isZoneOpen(1));
  // Serial.println(isZoneOpen(2));
  // delay(5000);

  // ---- TEST: ISO_END and ZCR ---- //
  // tempUpdated(1,25); // both should be off
  // delay(5000);
  // tempUpdated(1,10); //requesting heat and zone should turn on, so both ISO_END and ZCR should be turned on
  // delay(5000);
  // tempUpdated(3, 60); // cooling, so ISO_END should be off, and zone (ZCR) should be on
  // delay(5000);
  // tempUpdated(3, 10); // both off
  // delay(5000);
}

void testFanCoil(){
  // ---- TEST : tempUpdated and turn on/off valves ---- //
  // tempUpdated(1, 40);
  // delay(5000);
  // tempUpdated(1, 10);
  // delay(5000);

  // ---- TEST: get thermistor temp ---- //
  // Serial.println(getThermistorTemp("thermistorName"));
  // Serial.println(getThermistorTemp("secondThermistorName"));

  // ---- TEST : Update Set Point ---- //
  // Serial.println("Updating set point");
  // updateSetPoint(45, 0, "fanCoilName");
  // delay(5000);
  // tempUpdated(2, 50); // should be off
  // delay(5000);
  // tempUpdated(2, 10); // should be on
  // delay(15000);
}

void loop(){
  testFanCoil();
}

