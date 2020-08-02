#pragma once

#include "Screen.h"

typedef uint32_t (*DwellAdjustCallback_t)(int, int32_t);

class DwellAdjustScreen : public Screen
{
public:
    void Draw(LiquidCrystal& lcd);
    void JoyStickEventHandler(JoystickEvent event);
    void SetDwellAdjustCallback(DwellAdjustCallback_t funct);
    void SetCurrentBarrelDwell(int barrel, uint32_t dwell);

private:
    DwellAdjustCallback_t dwell_adjust_callback;
    uint32_t current_dwell;
    int barrel;

};