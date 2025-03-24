#include "tasks.h"

// #if WEBSERVER_ENABLED == true
void task_IOControl(void *parameters){
    delay(200); // short delay for board to be initialized

    controlSetup();

    delay(10);
    for (;;)
    {
        controlLoop();
        delay(10);
    }
}
// #endif

void setup_tasks(){
    xTaskCreate(
        task_IOControl,
        "CONTROL",
        512 * 10,
        NULL,
        1000,
        NULL
    );

}