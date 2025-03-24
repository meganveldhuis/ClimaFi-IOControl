#include <Arduino.h>

#include "IOControl/IOControl.h"

void setup(){

  Serial.begin(115200);

  controlSetup();

//     delay(10);
//     for (;;)
//     {
//         controlLoop();
//         delay(10);
//     }

  Serial.println("Setup complete");
}

void loop()
{
    //nc_loop();

    delay(5);
}