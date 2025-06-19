#include "thermistorPort.h"

thermistorPort::thermistorPort(String name, uint8_t number, uint8_t id){
    this->name = name;
    this->id = id;
    this->number = number;
    if(number == 1){
        _pin = THERMISTOR_0_PIN;
    }else if(number == 2){
        _pin = THERMISTOR_1_PIN;
    }else{
        //ERROR
        _pin = 0;
    }
    pinMode(_pin, INPUT);
}

float thermistorPort::getTemp(){
    if(_pin ==0){
        Serial.println("No pin found. Aborting.");
        return 0; //error
    }
    float a = 639.5f, b = -0.1332f, c = -162.5f;
    float Rntc, Vntc, TempC, TempF;
    Vntc = (analogRead(_pin) * 5.0f) / 4096.0f;
    Rntc = 10000.0f * ((5.0f / Vntc) - 1);
    TempC = a * pow(Rntc, b) + c - 2.0f;
    TempF = (TempC * 1.8f) + 32.0f;

    if (fabs(TempC - 403.28) < 0.01 || fabs(TempF - 757.90) < 0.01) {
        Serial.printf("Error: Thermistor %s not connected\n", name.c_str());
    }

    return TempF;
}

ADCThermistor::ADCThermistor(String name, uint8_t number, uint8_t id, TwoWire* I2CBus, uint8_t i2cAddress)
    : thermistorPort(name, number, id)
{
    if (!ads.begin(i2cAddress, I2CBus)) {
        Serial.printf("Failed to initialize ADS1115 at address 0x%02X for thermistor %s\n", i2cAddress, name.c_str());
    } else {
        Serial.printf("ADS1115 at address 0x%02X initialized for thermistor %s\n", i2cAddress, name.c_str());
    }
//     ads = Adafruit_ADS1115();
//     I2CBus.begin(I2C_SDA, I2C_SCL);
//     if(!ads.begin(0x48, &I2CBus)){
//         Serial.println("Failed to initialize ADS.");
//     }
}

float ADCThermistor::getTemp(){
    uint8_t channel = (number - 1) % 4;  // Map 1-4 to 0-3, and 5-8 to 0-3
    int16_t adcValue = ads.readADC_SingleEnded(channel);
    if (adcValue == 0) {
        Serial.printf("Warning: ADC value is 0 for thermistor %s on channel %d\n", name.c_str(), channel);
    }
    float volts = ads.computeVolts(adcValue);

    float a = 639.5f, b = -0.1332f, c = -162.5f;
    float Rntc, Vntc, TempC, TempF;
    Vntc = volts;
    Rntc = 10000.0f * ((3.3f / Vntc) - 1);
    TempC = a * pow(Rntc, b) + c - 2.0f;

    if (fabs(TempC - 403.28) < 0.01 || fabs(TempF - 757.90) < 0.01 || isnan(TempC) ){
        Serial.printf("Error: Thermistor %s not connected\n", name);
        return 0;
    }

    // ----- calculate ntc temp in Celsius using steinhart method ----- //
    uint16_t R1=10000;
    float Vcc = 3.3;
   
    // voltage divider : R2=Vr2*R1/(Vcc-Vr2)
    Rntc = volts*R1/(Vcc-volts);

    // Steinhart-Hart equation:
        // 1/T = A + B*ln(R) + C*ln(R)^2
        // https://www.arroyoinstruments.com/thermistors/
    float A = 1.12924e-3;
    float B = 2.34108e-4;
    float C = 0.87755e-7;

    float logR = log(Rntc);
    float steinhart = A + (B * logR) + (C * pow(logR,3));
    float tempK = 1.0 / steinhart;

    float TempC2 = tempK - 273.15;

    Serial.printf("Thermistor %s read either %.2fC or %.2fC\n", name, TempC, TempC2);

    return TempC2;
}