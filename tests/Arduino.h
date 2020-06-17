#pragma once
#include <memory>

#define HIGH 1
#define LOW 0
#define OUTPUT true
#define INPUT false

#define PIN_COUNT 100

//Arduino simulated functions
bool digitalRead(int pin);
void digitalWrite(int pin, bool state);
uint16_t analogRead(int pin);
void analogWrite(int pin, uint16_t state);
void pinMode(int pin, int mode);

class Arduino
{
public:
    uint16_t GetPinState(int pin) const;
    void SetPinState(int pin, uint16_t state);
    bool GetPinMode(int pin) const;
    void SetPinMode(int pin, bool mode);

private:
    uint16_t pin_states[PIN_COUNT];
    bool pin_modes[PIN_COUNT];
};

static Arduino arduino;
