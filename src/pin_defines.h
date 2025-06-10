#ifndef PIN_DEFINES_H
#define PIN_DEFINES_H

// -- -- -- -- -- -- -- VALVE CONTROLLER: -- -- -- -- -- -- -- //
#define ISO_END_PIN 12

#define ZN1_OUT_PIN 42
#define ZN2_OUT_PIN 41
#define ZN3_OUT_PIN 40
#define ZN4_OUT_PIN 39

#define ZN1_SENSE_PIN 38
#define ZN2_SENSE_PIN 37
#define ZN3_SENSE_PIN 36
#define ZN4_SENSE_PIN 35


// -- -- -- -- -- -- -- PUMP CONTROLLER: -- -- -- -- -- -- -- //
#define ZCR_OUT_PIN 41
#define ISO_IN_PIN 42

#define ZN1_IN_PIN 40
#define ZN2_IN_PIN 39
#define ZN3_IN_PIN 38

#define ZN1_SENSE_PUMP_PIN 35
#define ZN2_SENSE_PUMP_PIN 36
#define ZN3_SENSE_PUMP_PIN 37


// -- -- -- -- -- -- -- FAN COIL CONTROLLER: -- -- -- -- -- -- -- //
#define COIL_PWM_PIN 5

#define THERMISTOR_0_PIN 6
#define THERMISTOR_1_PIN 7


// -- -- -- -- -- -- -- HEAT PUMP CONTROLLER: -- -- -- -- -- -- -- //
//pump/valves
#define AUX1_IN_PIN 14
#define AUX2_IN_PIN 21
#define AUX3_IN_PIN 47
#define AUX4_IN_PIN 48

#define REV_IN_PIN 45

//thermostat demand
#define DMD1_SENSE_PIN 36
#define DMD2_SENSE_PIN 37

#define STG1_IN_PIN 4
#define STG2_IN_PIN 5
#define STG3_IN_PIN 6
#define STG4_IN_PIN 7

#endif