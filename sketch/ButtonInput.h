#pragma once
#include "InputDebouncer.h"
#include "Arduino.h"

class ButtonInput
{
public:
    ButtonInput(unsigned int _input_pin, uint16_t debounce_time = 250) : input_pin(_input_pin), debouncer(debounce_time){}

    bool IsPressed(uint32_t current_time)
    {
        if (HIGH == debouncer.DebounceInput(digitalRead(input_pin), current_time))
        {
            if (waiting_for_falling_edge)
                return false;
            waiting_for_falling_edge = true;
            return true;
        }
        else
        {
            waiting_for_falling_edge = false;
        }
        return false;
    }

private:
    int input_pin;
    InputDebouncer debouncer;
    bool waiting_for_falling_edge = false;
    
};