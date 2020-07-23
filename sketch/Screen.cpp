#include "Screen.h"


void Screen::SetFocused(bool val)
{
    if (val && joystick)
    {
        joystick->SetEventHandler(JoyStickEventHandler);
    }
}