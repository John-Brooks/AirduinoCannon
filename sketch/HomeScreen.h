#pragma once
#include "Screen.h"

class HomeScreen : public Screen
{
public:
    static bool GetSafetyOn() { return HomeScreen::safety_on; }
    void Draw(LiquidCrystal& lcd);
    void JoyStickEventHandler(JoystickEvent event);
    void SetSelectedBarrel(int barrel);

private:
    float CalculatePressure(int analog_value);

    static bool safety_on;
    int selected_barrel = 1;
    float pressure = 0.0;    
};