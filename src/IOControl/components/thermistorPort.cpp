#include "thermistorPort.h"


thermistorPort::thermistorPort(String name, int number){
    this->name = name;
    if(number == 1){
        _pin = THERMISTOR_0_PIN;
    }else if(number == 2){
        _pin = THERMISTOR_1_PIN;
    }else{
        //ERROR
        _pin = 0;
    }
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

    return TempF;
}