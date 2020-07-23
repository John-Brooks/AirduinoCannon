#include "gtest/gtest.h"
#include "Joystick.h"
#include "HomeScreen.h"
#include "Arduino.h"
#include "ArduinoPins.h"
#include "main.h"
#include "PhysicalConstants.h"
#include "LiquidCrystal.h"
#include <string>

int ConvertPressureToAIN(float pressure)
{
    float voltage = pressure / PSI_PER_VOLT;
    return voltage * ((float)AIN_MAX_VALUE / AIN_MAX_VOLT);
}
void SetPressure(float pressure)
{
    arduino.SetPinState(PRESSURE_TRANSD_PIN, ConvertPressureToAIN(pressure + 0.05));
}

TEST(HomeScreenTests, SafetyOnAtStartup)
{
    HomeScreen uut;
    ASSERT_TRUE(uut.GetSafetyOn());
}

TEST(HomeScreenTests, PressureRead)
{   
    arduino.Reset();
    InitializePins();
    HomeScreen uut;
    LiquidCrystal lcd(12, 13, 8, 9, 10, 11);
    lcd.begin(16,2);
    
    SetPressure(46.2);
    uut.Draw(lcd);
    lcd.PrintToConsole();
    std::string row = lcd.GetRow(1);
    EXPECT_TRUE(row.find("Pressure") != std::string::npos);
    EXPECT_TRUE(row.find("46.2") != std::string::npos);

    SetPressure(120.5);
    uut.Draw(lcd);
    lcd.PrintToConsole();
    row = lcd.GetRow(1);
    EXPECT_TRUE(row.find("Pressure") != std::string::npos);
    EXPECT_TRUE(row.find("120.5") != std::string::npos);
}

TEST(HomeScreenTests, BarrelSelect)
{ 
    arduino.Reset();
    InitializePins();
    HomeScreen uut;
    LiquidCrystal lcd(12, 13, 8, 9, 10, 11);
    lcd.begin(16,2);
    SetPressure(46.2);

    uut.SetSelectedBarrel(1);
    uut.Draw(lcd);
    lcd.PrintToConsole();
    std::string row = lcd.GetRow(0);
    EXPECT_TRUE(row.find("2\"") != std::string::npos);

    uut.SetSelectedBarrel(2);
    uut.Draw(lcd);
    lcd.PrintToConsole();
    row = lcd.GetRow(0);
    EXPECT_TRUE(row.find("1\"") != std::string::npos);

    uut.SetSelectedBarrel(3);
    uut.Draw(lcd);
    lcd.PrintToConsole();
    row = lcd.GetRow(0);
    EXPECT_TRUE(row.find("1/8\"") != std::string::npos);
}

TEST(HomeScreenTests, Safety)
{ 
    arduino.Reset();
    InitializePins();
    HomeScreen uut;
    LiquidCrystal lcd(12, 13, 8, 9, 10, 11);
    lcd.begin(16,2);
    SetPressure(46.2);

    uut.Draw(lcd);
    lcd.PrintToConsole();
    std::string row = lcd.GetRow(0);
    EXPECT_TRUE(row.find("SAFE") != std::string::npos);
    EXPECT_TRUE(uut.GetSafetyOn());

    uut.JoyStickEventHandler(JoystickEvent::Click);
    uut.Draw(lcd);
    lcd.PrintToConsole();
    row = lcd.GetRow(0);
    EXPECT_TRUE(row.find("FIRE") != std::string::npos);
    EXPECT_FALSE(uut.GetSafetyOn());

    uut.JoyStickEventHandler(JoystickEvent::Click);
    uut.Draw(lcd);
    lcd.PrintToConsole();
    row = lcd.GetRow(0);
    EXPECT_TRUE(row.find("SAFE") != std::string::npos);
    EXPECT_TRUE(uut.GetSafetyOn());
}
