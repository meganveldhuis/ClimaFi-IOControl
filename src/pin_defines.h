#ifndef PIN_DEFINES_H
#define PIN_DEFINES_H

//commented pins are for ESP32-S3

// -- -- -- -- -- -- -- VALVE CONTROLLER: -- -- -- -- -- -- -- //
#define ISO_END_PIN 12

#define ZN1_OUT_PIN 42
// #define ZN1_OUT_PIN 25;
#define ZN2_OUT_PIN 41
// #define ZN2_OUT_PIN 26;
#define ZN3_OUT_PIN 40
#define ZN4_OUT_PIN 39

// -- -- -- -- -- -- -- PUMP CONTROLLER: -- -- -- -- -- -- -- //
#define ZCR_OUT_PIN 41
// #define ZCR_OUT_PIN 25;

#define ZN1_IN_PIN 40
#define ZN2_IN_PIN 39
#define ZN3_IN_PIN 38
// #define ZN3_IN_PIN 26;


// -- -- -- -- -- -- -- FAN COIL CONTROLLER: -- -- -- -- -- -- -- //
#define COIL_PWM_PIN 5

#define THERMISTOR_0_PIN 6
#define THERMISTOR_1_PIN 7


#endif