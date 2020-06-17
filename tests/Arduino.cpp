#include "Arduino.h"
#include <stdexcept>

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