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
