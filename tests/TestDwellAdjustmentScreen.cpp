#include "gtest/gtest.h"
#include "Joystick.h"
#include "DwellAdjustScreen.h"
#include "StepperMotorSim.h"
#include "Arduino.h"
#include "ArduinoPins.h"
#include "main.h"
#include "PhysicalConstants.h"
#include "LiquidCrystal.h"
#include <string>
#include <tuple>

int32_t last_dwell_increment = 0;
int last_barrel = 0;
uint32_t dwell = 0;
static uint32_t DwellAdjustCallback(int barrel, int32_t increment)
{
    //Store the values for validation
    last_dwell_increment = increment;
    last_barrel = barrel;

    //perform same dwell constraint as main.cpp, this should probably exist within
    //fire control :/
    if(increment < 0 && (-increment) >= dwell)
        dwell = 1;
    else
        dwell += increment;
    
    return dwell;
}
static auto Init()
{
    struct retVals{
        DwellAdjustScreen screen;
        LiquidCrystal lcd;
    } ret;

    arduino.Reset();
    InitializePins();
    ret.lcd.begin(16,2);
    ret.screen.SetDwellAdjustCallback(DwellAdjustCallback);
    return ret;
}
static void SetDwell(DwellAdjustScreen& uut, uint32_t new_dwell)
{
    dwell = new_dwell;
    uut.SetCurrentBarrelDwell(last_barrel, new_dwell);
}
static void SetBarrel(DwellAdjustScreen& uut, int new_barrel)
{
    last_barrel = new_barrel;
    uut.SetCurrentBarrelDwell(new_barrel, dwell);
}
static std::string DrawAndGetLine(DwellAdjustScreen& uut, LiquidCrystal& lcd, int row)
{
    uut.Draw(lcd);
    return lcd.GetRow(row);
}
static bool CheckForTimeText(DwellAdjustScreen& uut, LiquidCrystal& lcd)
{
    std::string line = DrawAndGetLine(uut, lcd, 1);
    return line.find("Time:") != std::string::npos;
}
static bool CheckForDwellValueText(DwellAdjustScreen& uut, LiquidCrystal& lcd, std::string dwell_value)
{
    std::string line = DrawAndGetLine(uut, lcd, 1);
    return line.find(dwell_value) != std::string::npos;
}

TEST(DwellAdjustmentTests, SetDwell)
{
    auto [uut, lcd] = Init();

    SetDwell(uut, 247);
    EXPECT_TRUE(CheckForTimeText(uut, lcd));
    EXPECT_TRUE(CheckForDwellValueText(uut, lcd, "0.247 ms"));
    lcd.PrintToConsole();

    SetDwell(uut, 1'500);
    EXPECT_TRUE(CheckForTimeText(uut, lcd));
    EXPECT_TRUE(CheckForDwellValueText(uut, lcd, "1.500 ms"));
    lcd.PrintToConsole();

    SetDwell(uut, 3'567);
    EXPECT_TRUE(CheckForTimeText(uut, lcd));
    EXPECT_TRUE(CheckForDwellValueText(uut, lcd, "3.567 ms"));
    lcd.PrintToConsole();

    SetDwell(uut, 1'000'000);
    EXPECT_TRUE(CheckForTimeText(uut, lcd));
    EXPECT_TRUE(CheckForDwellValueText(uut, lcd, "1.000 s"));
    lcd.PrintToConsole();

    SetDwell(uut, 1'003'567);
    EXPECT_TRUE(CheckForTimeText(uut, lcd));
    EXPECT_TRUE(CheckForDwellValueText(uut, lcd, "1.004 s"));
    lcd.PrintToConsole();

    SetDwell(uut, 10'123'567);
    EXPECT_TRUE(CheckForTimeText(uut, lcd));
    EXPECT_TRUE(CheckForDwellValueText(uut, lcd, "10.124 s"));
    lcd.PrintToConsole();

    SetDwell(uut, 1'999'123'567);
    EXPECT_TRUE(CheckForTimeText(uut, lcd));
    EXPECT_TRUE(CheckForDwellValueText(uut, lcd, "1999.124 s"));
    lcd.PrintToConsole();
}

TEST(DwellAdjustmentTests, IncrementUpDown)
{   
    auto [uut, lcd] = Init();

    SetDwell(uut, 247);
    uut.JoyStickEventHandler(JoystickEvent::UpLarge);
    EXPECT_TRUE(CheckForDwellValueText(uut, lcd, "0.271 ms"));
    lcd.PrintToConsole();

    uut.JoyStickEventHandler(JoystickEvent::UpSmall);
    EXPECT_TRUE(CheckForDwellValueText(uut, lcd, "0.273 ms"));
    lcd.PrintToConsole();

    uut.JoyStickEventHandler(JoystickEvent::UpLarge);
    EXPECT_TRUE(CheckForDwellValueText(uut, lcd, "0.300 ms"));
    lcd.PrintToConsole();

    SetDwell(uut, 1'003'567);
    uut.JoyStickEventHandler(JoystickEvent::UpLarge);
    EXPECT_TRUE(CheckForDwellValueText(uut, lcd, "1.104 s"));
    lcd.PrintToConsole();

    uut.JoyStickEventHandler(JoystickEvent::DownLarge);
    EXPECT_TRUE(CheckForDwellValueText(uut, lcd, "993.531 ms"));
    lcd.PrintToConsole();

    uut.JoyStickEventHandler(JoystickEvent::DownSmall);
    EXPECT_TRUE(CheckForDwellValueText(uut, lcd, "983.596 ms"));
    lcd.PrintToConsole();

    SetDwell(uut, 10);
    uut.JoyStickEventHandler(JoystickEvent::DownSmall);
    EXPECT_TRUE(CheckForDwellValueText(uut, lcd, "0.009 ms"));
    lcd.PrintToConsole();

    SetDwell(uut, 2);
    uut.JoyStickEventHandler(JoystickEvent::DownSmall);
    EXPECT_TRUE(CheckForDwellValueText(uut, lcd, "0.001 ms"));
    lcd.PrintToConsole();

    uut.JoyStickEventHandler(JoystickEvent::DownLarge);
    EXPECT_TRUE(CheckForDwellValueText(uut, lcd, "0.001 ms"));
    lcd.PrintToConsole();
}
