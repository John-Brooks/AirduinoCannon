#pragma once
#include <memory>
#include <map>
#include <vector>
#include <tuple>
#include "Mega2560Pins.h"
#include <stdint.h>

#define HIGH 1
#define LOW 0
#define OUTPUT true
#define INPUT false
#define CHANGE 2
#define RISING 3
#define FALLING 4

//Interrupt service routine function pointer
typedef void (*ISR)(); 

//Arduino simulated functions
bool digitalRead(int pin);
void digitalWrite(int pin, bool state);
uint16_t analogRead(int pin);
void analogWrite(int pin, uint16_t state);
void pinMode(int pin, int mode);
uint32_t millis();
uint32_t micros();
void attachInterrupt(int interrupt, ISR funct, int mode);
void detachInterrupt(int interrupt);
int digitalPinToInterrupt(int pin);
void delay(int milliseconds);
void delayMicroseconds(int microseconds);

class Arduino
{
public:
    void IncrementOnMicrosCall(bool set) {increment_on_micros_call = set;}
    void TickMillisecond(uint32_t milliseconds = 1);
    void TickMicrosecond(uint32_t microseconds = 1);
    void Reset();
    uint16_t GetPinState(int pin) const;
    void SetPinState(int pin, uint16_t state);
    bool GetPinMode(int pin) const;
    void SetPinMode(int pin, bool mode);
    uint32_t Millis();
    uint32_t Micros();
    void SchedulePinToggle(int pin, uint32_t microseconds_offset);
    void AttachInterrupt(int interrupt, ISR funct, int mode);
    void DetachInterrupt(int pin);

private:
    void HandlePinInterrupt(int pin, int old_value, int new_value);

    uint16_t pin_states[PIN_COUNT];
    bool pin_modes[PIN_COUNT];
    uint64_t current_micros = 0;
    bool increment_on_micros_call = false;
    std::map<uint16_t, uint8_t> registers;
    std::map<int, std::pair<ISR, int> > interrupts;

    //used for simulating pin state changes on an arbitrary timeline.
    std::map<uint64_t, std::vector<int>> scheduled_pin_toggles;
};

extern Arduino arduino;
