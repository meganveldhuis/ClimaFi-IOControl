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

ADCThermistor::ADCThermistor(String name, uint8_t number, uint8_t id)
    : thermistorPort(name, number, id), I2CBus(0)
{
    ads = Adafruit_ADS1115();
    I2CBus.begin(17, 18);
    if(!ads.begin(0x48, &I2CBus)){
        Serial.println("Failed to initialize ADS.");
    }
}

float ADCThermistor::getTemp(){
    int16_t adc;
    float volts;

    adc = ads.readADC_SingleEnded(number-1);

    volts = ads.computeVolts(adc);

    Serial.print("AIN1: "); Serial.print(adc); Serial.print("  "); Serial.print(volts); Serial.println("V");

    float v = adc * (3.3 / 65535);
    Serial.println(v);
    v = adc * 0.0001875;     // Using default TwoThirds gain = 0.1875mV per 1-bit
    Serial.println(v);

    float a = 639.5f, b = -0.1332f, c = -162.5f;
    float Rntc, Vntc, TempC, TempF;
    Vntc = (volts * 3.3f) / 4096.0f;
    Rntc = 10000.0f * ((3.3f / Vntc) - 1);
    TempC = a * pow(Rntc, b) + c - 2.0f;

    if (fabs(TempC - 403.28) < 0.01 || fabs(TempF - 757.90) < 0.01) {
        Serial.printf("Error: Thermistor not connected\n");
    }
    Serial.printf("Thermistor 1 read ");
    Serial.println(TempC);
    return TempC;
}