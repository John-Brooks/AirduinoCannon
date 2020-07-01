#include "Arduino.h"
#include <stdexcept>

Arduino arduino;

namespace Validate
{
    void PinExists(int pin)
    {
        if (pin < 0 || pin >= PIN_COUNT)
            throw std::out_of_range("Invalid pin number");
    }
    void PinIsInput(int pin)
    {
        if (arduino.GetPinState(pin) != INPUT)
            throw std::logic_error("Pin is not set to input");
    }
    void PinIsOutput(int pin)
    {
        if (arduino.GetPinState(pin) != OUTPUT)
            throw std::logic_error("Pin is not set to output");
    }
    void PinHasADC(int pin)
    {
        if(pin < A0 || pin > A15)
            throw std::out_of_range("Pin does not support analogRead");
    }
    void PinHasPWM(int pin)
    {
        if(PWM_PINS.find(pin) == PWM_PINS.end())
            throw std::out_of_range("Pin does not support analogWrite");
    }
}

bool digitalRead(int pin)
{
    Validate::PinExists(pin);
    Validate::PinIsInput(pin);
    return (bool)arduino.GetPinState(pin);
}
uint16_t analogRead(int pin)
{
    Validate::PinExists(pin);
    Validate::PinIsInput(pin);
    Validate::PinHasADC(pin);
    return arduino.GetPinState(pin);
}
void digitalWrite(int pin, bool state)
{
    Validate::PinExists(pin);
    Validate::PinIsOutput(pin);
    arduino.SetPinState(pin, state);
}
void analogWrite(int pin, uint16_t state)
{
    Validate::PinExists(pin);
    Validate::PinIsOutput(pin);
    arduino.SetPinState(pin, state);
}
uint32_t millis()
{
    return arduino.Millis();
}
uint32_t micros()
{
    return arduino.Micros();
}
void pinMode(int pin, int mode)
{
    arduino.SetPinMode(pin, mode);
}


void Arduino::TickMillisecond(uint32_t milliseconds)
{
    current_micros += ((uint64_t)milliseconds) * 1000;
    CheckPinSchedule();
}
void Arduino::TickMicrosecond(uint32_t microseconds)
{
    current_micros += microseconds;
    CheckPinSchedule();
}
void Arduino::CheckPinSchedule()
{
    if(!scheduled_pin_toggles.size())
        return;
    //I just want my c++17 structured bindings :'(
    for(const auto& [time, pins] : scheduled_pin_toggles)
    {
        //std::map is sorted so we can safely return if we haven't reached this time.
        if (time < current_micros)
            return;

        for(const auto& pin : pins)
            SetPinState(pin, !GetPinState(pin));

        scheduled_pin_toggles.erase(time);
    }
}
uint32_t Arduino::Micros()
{
    if(increment_on_micros_call)
        current_micros += 4;

    return current_micros & 0xFFFFFFFF;
}
uint32_t Arduino::Millis()
{
    return (current_micros / 1000) & 0xFFFFFFFF;
}
uint16_t Arduino::GetPinState(int pin) const
{
    return pin_states[pin];
}
void Arduino::SetPinState(int pin, uint16_t state)
{
    pin_states[pin] = state; 
}
bool Arduino::GetPinMode(int pin) const
{
    return pin_modes[pin];
}
void Arduino::SetPinMode(int pin, bool mode)
{
    pin_modes[pin] = mode;
}
bool Arduino::Reset()
{
    return false;
}

