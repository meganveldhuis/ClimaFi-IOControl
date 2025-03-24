#include <Arduino.h>

#include "tasks/tasks.h"

void setup(){

  Serial.begin(115200);

  setup_tasks();

  Serial.println("Setup complete");
}

void loop()
{
    //nc_loop();

    delay(5);
}