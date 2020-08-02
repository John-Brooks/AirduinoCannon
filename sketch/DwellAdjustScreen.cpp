#include "DwellAdjustScreen.h"

template <class T>
static T max(T a, T b)
{
    if( a > b )
        return a;
    else
        return b;
}

template <class T>
static T min(T a, T b)
{
    if( a < b )
        return a;
    else
        return b;
}

void DwellAdjustScreen::Draw(LiquidCrystal& lcd)
{
    lcd.clear();
    lcd.setCursor(0,0);
    char buffer[17];
    sprintf(buffer, "Dwell Adjust B:%d", barrel);
    lcd.print(buffer);

    lcd.setCursor(0,1);
    if(current_dwell >= 1000000) //seconds range
        sprintf(buffer, "Time: %6.3f s", ((double)current_dwell / 1000000.0));
    else
        sprintf(buffer, "Time: %6.3f ms", ((double)current_dwell / 1000.0));
    
    lcd.print(buffer);
}
void DwellAdjustScreen::JoyStickEventHandler(JoystickEvent event)
{
    int32_t increment_size;
    switch(event)
    {
        case JoystickEvent::DownLarge:
            increment_size = min(current_dwell * -0.1, -1.0);
            break;
        case JoystickEvent::UpLarge:
            increment_size = max(current_dwell * 0.1, 1.0);
            break;
        case JoystickEvent::DownSmall:
            increment_size = min(current_dwell * -0.01, -1.0);
            break;
        case JoystickEvent::UpSmall:
            increment_size = max(current_dwell * 0.01, 1.0);
            break;
        default:
            return;
    }

    current_dwell = dwell_adjust_callback(barrel, increment_size);
}
void DwellAdjustScreen::SetDwellAdjustCallback(DwellAdjustCallback_t funct)
{
    dwell_adjust_callback = funct;
}

void DwellAdjustScreen::SetCurrentBarrelDwell(int new_barrel, uint32_t new_dwell)
{
    barrel = new_barrel;
    current_dwell = new_dwell;
}