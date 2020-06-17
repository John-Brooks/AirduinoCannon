#pragma once
#include "InputDebouncer.h"
#include "Arduino.h"

class DigitalInput
{
public:
    DigitalInput(unsigned int _input_pin, uint16_t debounce_time) : input_pin(_input_pin), debouncer(false, debounce_time){}


private:
    InputDebouncer debouncer;
    unsigned int input_pin;
}