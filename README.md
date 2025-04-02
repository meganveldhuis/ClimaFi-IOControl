# IO Control for ClimaFi System

Input/Output Control for an HVAC system to be used in ESP32-S3 projects. This requires a settings.json with the same structure as the file in data.

## Components

### Zone Valve Controller

- zoneOutput
- end Switch (thermostatEndSwitch)

#### Pins:

1. ISO_END : 12
2. ZN1_OUT : 42
3. ZN2_OUT : 41
4. ZN3_OUT : 40
5. ZN4_OUT : 39

### Zone Pump Controller

- zoneOutput
- end Switch (zoneEndSwitch and thermostatEndSwitch)

#### Pins:

1. ZCR_OUT : 12
2. ISO_IN : 42
3. ZN1_IN : 40
4. ZN2_IN : 39
5. ZN3_IN : 38

### Fan Coil Controller

- ADC Output
- thermistorPort

#### Pins:

1. COIL_PWM : 5
2. THERMISTOR_0 : 6
3. THERMISTOR_1 : 7
