#include "thermistorPort.h";


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