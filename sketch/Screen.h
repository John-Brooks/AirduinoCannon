#pragma once

#include "LiquidCrystal.h"
#include "Joystick.h"

class Screen
{
public:
    virtual void Draw(LiquidCrystal& lcd) = 0;
    virtual void JoyStickEventHandler(JoystickEvent event) = 0;

};