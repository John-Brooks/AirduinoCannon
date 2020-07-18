#pragma once
#include <stdint.h>

class InputDebouncer
{
public:
    InputDebouncer(uint16_t _debounce_time_ms = 250) : debounce_time(_debounce_time_ms) {}
    
    //pass in the current value and current time, returns the debounced value
    bool DebounceInput(bool real_value, uint32_t current_time_ms)
    {        
        //the values are different and we're beyond the debouncing time || this is the first time calling.
        if( (real_value != debounced_value && (current_time_ms - last_input_time > (uint32_t)debounce_time)) || last_input_time == 0)
        {
            //update the debounced value.
            last_input_time = current_time_ms;
            debounced_value = real_value;
        }
        return debounced_value;
    }
    
private:
    bool debounced_value;
    uint32_t last_input_time = 0;
    uint16_t debounce_time;
};