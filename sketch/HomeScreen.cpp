#include "HomeScreen.h"
#include "Arduino.h"
#include "ArduinoPins.h"
#include "PhysicalConstants.h"

#if defined (__linux__) || (__WIN32)
    #include <cassert>
#else
    #include <assert.h>
#endif

bool HomeScreen::safety_on = true;
char text_buffer[17];

float HomeScreen::CalculatePressure(int analog_value)
{
    float voltage = ((float)analog_value / (float)(AIN_MAX_VALUE)) * AIN_MAX_VOLT;
    return voltage * PSI_PER_VOLT;
}

void HomeScreen::JoyStickEventHandler(JoystickEvent event)
{
    switch (event)
    {
        case JoystickEvent::Click:
            HomeScreen::safety_on = !HomeScreen::safety_on;
            break;
        default:
            //ignore other joystick inputs
            return;
    }
}

void HomeScreen::Draw(LiquidCrystal& lcd)
{
    lcd.clear();
    lcd.setCursor(0,0);
    switch (selected_barrel)
    {
        case 1:
            lcd.print("Barrel: 2\"");
            break;
        case 2:
            lcd.print("Barrel: 1\"");
            break;  
        case 3:
            lcd.print("Barrel:1/8\"");
            break; 
        default:
            lcd.print("Barrel: ERROR");
            break;
    }

    static const int safe_pos = 12;
    lcd.setCursor(safe_pos,0);
    if(safety_on)
        lcd.print("SAFE");
    else
        lcd.print("FIRE");

    lcd.setCursor(0,1);
    lcd.print("Pressure");

    float pressure = CalculatePressure(analogRead(PRESSURE_TRANSD_PIN));
    sprintf(text_buffer, "%3.1fpsi", pressure);

    if(pressure < 100)
        lcd.setCursor(9,1);
    else
        lcd.setCursor(8,1);
    
    lcd.print(text_buffer);
}

void HomeScreen::SetSelectedBarrel(int barrel)
{
    assert(barrel >= 1 && barrel <= 3);
    selected_barrel = barrel;
}
