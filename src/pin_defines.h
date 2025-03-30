#ifndef PIN_DEFINES_H
#define PIN_DEFINES_H

//commented pins are for ESP32-S3

// -- -- -- -- -- -- -- VALVE CONTROLLER: -- -- -- -- -- -- -- //
// #define ZN1_OUT_PIN 35;
#define ZN1_OUT_PIN 25;
// #define ZN2_OUT_PIN 34;
#define ZN2_OUT_PIN 26;
#define ZN3_OUT_PIN 33;
#define ZN4_OUT_PIN 32;

// -- -- -- -- -- -- -- PUMP CONTROLLER: -- -- -- -- -- -- -- //
// #define ZCR_OUT_PIN 34;
#define ZCR_OUT_PIN 25;

#define ZN1_IN_PIN 33;
#define ZN2_IN_PIN 32;
// #define ZN3_IN_PIN 31;
#define ZN3_IN_PIN 26;


// -- -- -- -- -- -- -- FAN COIL CONTROLLER: -- -- -- -- -- -- -- //
#define COIL_PWM_PIN 5;

#define THERMISTOR_0_PIN 7;
#define THERMISTOR_1_PIN 6;


#endif