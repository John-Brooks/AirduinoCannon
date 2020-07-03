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
        if (arduino.GetPinMode(pin) != INPUT)
            throw std::logic_error("Pin is not set to input");
    }
    void PinIsOutput(int pin)
    {
        if (arduino.GetPinMode(pin) != OUTPUT)
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
void delay(int milliseconds)
{
    if ((((uint64_t)milliseconds)*1000) > 0xFFFFFFFF)
        throw std::out_of_range("simulator only supports delays up to ~35.8 minutes");
    arduino.TickMicrosecond(milliseconds * 1000);
}
void delayMicroseconds(int microseconds)
{
    arduino.TickMicrosecond(microseconds);
}

void attachInterrupt(int interrupt, ISR funct, int mode)
{
    arduino.AttachInterrupt(interrupt, funct, mode);
}
void detachInterrupt(int interrupt)
{
    arduino.DetachInterrupt(interrupt);
}


void Arduino::TickMillisecond(uint32_t milliseconds)
{
    if ((((uint64_t)milliseconds)*1000) > 0xFFFFFFFF)
        throw std::out_of_range("simulator only supports millisecond ticks up to ~35.8 minutes");
    TickMicrosecond(milliseconds * 1000);
}

void Arduino::TickMicrosecond(uint32_t microseconds)
{
    uint64_t starting_time = current_micros;
    uint64_t ending_time = starting_time + microseconds;

    if(!scheduled_pin_toggles.size())
    {
        current_micros = ending_time;
        return;
    }

    auto first_iterator = scheduled_pin_toggles.begin();

    //If the first pin change is still beyond where we will be after "Ticking"
    if(first_iterator->first > ending_time)
    {
        current_micros = ending_time;
        return;
    }

    auto last_erase_iterator = first_iterator;
    auto end_iterator = scheduled_pin_toggles.end();
    for(auto it = first_iterator; it != end_iterator; it++)
    {
        const auto& [pin_change_time, pins] = *it;

        //std::map is sorted so we can safely stop if we haven't reached this time.
        if (pin_change_time > ending_time)
            break;

        //fast forward the clock to the pin change time
        current_micros = pin_change_time;

        for(const auto& pin : pins)
            SetPinState(pin, !GetPinState(pin));

        last_erase_iterator = it;
    }

    scheduled_pin_toggles.erase(first_iterator, ++last_erase_iterator);
    current_micros = ending_time;
}
void Arduino::SchedulePinToggle(int pin, uint32_t microseconds_offset)
{
    uint64_t scheduled_time = current_micros + microseconds_offset;
    scheduled_pin_toggles[scheduled_time].push_back(pin);
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
    HandlePinInterrupt(pin, pin_states[pin], state);
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
void Arduino::Reset()
{
    for(int i = 0; i < PIN_COUNT; i++)
    {
        pin_states[i] = LOW;
        pin_modes[i] = OUTPUT;
    }
    current_micros = 0;
    increment_on_micros_call = false;
    registers.clear();
    interrupts.clear();
    scheduled_pin_toggles.clear();
}
void Arduino::AttachInterrupt(int interrupt, ISR funct, int mode)
{
    interrupts[interrupt] = std::make_pair(funct, mode);
}
void Arduino::DetachInterrupt(int pin)
{
    interrupts.erase(pin);
}
void Arduino::HandlePinInterrupt(int pin, int old_value, int new_value)
{
    auto it = interrupts.find(pin);
    if(it == interrupts.end())
        return;

    const auto& [routine, mode] = it->second;

    switch (mode)
    {
        case HIGH:
            if (new_value == HIGH)
                routine();
            break;
        case LOW:
            if (new_value == LOW)
                routine();
            break;
        case RISING:
            if (new_value == HIGH && old_value == LOW)
                routine();
            break;
        case FALLING:
            if (new_value == LOW && old_value == HIGH)
                routine();
            break;
        case CHANGE:
            if (new_value != old_value)
                routine();
            break;
    }
}

