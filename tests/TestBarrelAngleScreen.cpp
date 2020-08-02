#include "gtest/gtest.h"
#include "Joystick.h"
#include "BarrelAngleScreen.h"
#include "StepperMotorSim.h"
#include "Arduino.h"
#include "ArduinoPins.h"
#include "main.h"
#include "PhysicalConstants.h"
#include "LiquidCrystal.h"
#include <string>
#include <tuple>

static auto Init()
{
    struct retVals{
        BarrelAngleScreen screen;
        LiquidCrystal lcd;
    } ret;

    arduino.Reset();
    InitializePins();
    ret.lcd.begin(16,2);
    return ret;
}

int BarrelAngleToAIN(double angle)
{
    double radians = (angle / 360) * (2 * PI);
    double gravities = sin(radians);
    double voltage = (gravities * ACCELL_SENSITIVITY) + ACCELL_BIAS;
    double ain_float_val = (voltage / AIN_MAX_VOLT) * AIN_MAX_VALUE;

    if(ain_float_val < 0)
        ain_float_val = 0;
    else if(ain_float_val > 1023)
        ain_float_val = 1023;

    return (int)ain_float_val;
}
bool CheckForAngle(float angle, std::string text)
{
    char text_buffer[10];
    sprintf(text_buffer, "%3.1f deg", angle);
    return text.find(text_buffer) != std::string::npos;
}

bool CheckForApproximateAngle(float angle, std::string text)
{
    std::string angle_text = text.substr(6, 6);
    double displayed_angle = std::stod(angle_text);
    double difference = displayed_angle - angle;
    return( difference < 1 && difference > -1);
}

void SetAngle(float angle)
{
    arduino.SetPinState(ACCELEROMETER_PIN, BarrelAngleToAIN(angle));
}

std::string DrawAndGetLine(BarrelAngleScreen& uut, LiquidCrystal& lcd)
{
    uut.Draw(lcd);
    lcd.PrintToConsole();
    return lcd.GetRow(1);
}

bool CheckAngleTextLine(BarrelAngleScreen& uut, LiquidCrystal& lcd, float angle)
{
    SetAngle(angle);
    std::string line = DrawAndGetLine(uut, lcd);

    if (!CheckForApproximateAngle(angle, line))
        return false;

    return line.find("Angle: ") != std::string::npos;
}

TEST(BarrelAngleScreenTests, AngleDisplay)
{
    auto [uut, lcd] = Init();

    //Note: This test will fail for large angle due to the limitations
    // of the A/D converter and the increasing sensitivity of the arcsin 
    // function as it approaches the edges of its domain, 1 and -1.

    EXPECT_TRUE(CheckAngleTextLine(uut, lcd, 1.0));
    EXPECT_TRUE(CheckAngleTextLine(uut, lcd, -1.0));
    EXPECT_TRUE(CheckAngleTextLine(uut, lcd, -45.0));
    EXPECT_TRUE(CheckAngleTextLine(uut, lcd, 30.5));
    EXPECT_TRUE(CheckAngleTextLine(uut, lcd, -30.5));
    EXPECT_TRUE(CheckAngleTextLine(uut, lcd, 0.0));
    EXPECT_TRUE(CheckAngleTextLine(uut, lcd, 15.7));
    EXPECT_TRUE(CheckAngleTextLine(uut, lcd, -15.7));
    EXPECT_TRUE(CheckAngleTextLine(uut, lcd, 60.5));
    EXPECT_TRUE(CheckAngleTextLine(uut, lcd, -60.5));
}


void Spin(BarrelAngleScreen& uut, StepperMotorSim& stepper, uint32_t milliseconds_to_spin)
{
    uint32_t current_time = millis();
    for(int i = 0; i < milliseconds_to_spin; i++)
    {
        uut.Run(current_time + i);
        stepper.Run();
        SetAngle(stepper.current_angle);
        arduino.TickMillisecond();
    }  
}

TEST(BarrelAngleScreenTests, ZeroAngle)
{
    auto [uut, lcd] = Init();
    StepperMotorSim stepper;
    SetAngle(15.0f);
    stepper.current_angle = 15.0f;
    uut.JoyStickEventHandler(JoystickEvent::Click);
    Spin(uut, stepper, 3100);
    EXPECT_NEAR(stepper.current_angle, 0.0f, 1);

    SetAngle(-15.0f);
    stepper.current_angle = -15.0f;
    uut.JoyStickEventHandler(JoystickEvent::Click);
    Spin(uut, stepper, 3100);
    EXPECT_NEAR(stepper.current_angle, 0.0f, 1);
}

int CalculateSeekTime(float delta_angle)
{
    if(delta_angle < 0)
        delta_angle *= -1;
    return (int)(delta_angle / DEGREE_PER_STEP) * MILLIS_BETWEEN_STEP;
}

bool TestAngleSeek(BarrelAngleScreen& uut, StepperMotorSim& stepper, float start, float end)
{
    stepper.current_angle = start;
    Spin(uut, stepper, CalculateSeekTime(end - start));
    return stepper.current_angle > end - 1 && stepper.current_angle < end + 1;
}

TEST(BarrelAngleScreenTests, StepUpAndDown)
{
    auto [uut, lcd] = Init();
    StepperMotorSim stepper;

    float start = 15.0f;

    SetAngle(start);
    uut.JoyStickEventHandler(JoystickEvent::DownLarge);
    EXPECT_TRUE(TestAngleSeek(uut, stepper, start, start - BARREL_DEG_PER_LARGE_STEP));

    SetAngle(start);
    uut.JoyStickEventHandler(JoystickEvent::DownSmall);
    EXPECT_TRUE(TestAngleSeek(uut, stepper, start, start - BARREL_DEG_PER_SMALL_STEP));

    SetAngle(start);
    uut.JoyStickEventHandler(JoystickEvent::UpLarge);
    EXPECT_TRUE(TestAngleSeek(uut, stepper, start, start + BARREL_DEG_PER_LARGE_STEP));

    SetAngle(start);
    uut.JoyStickEventHandler(JoystickEvent::UpSmall);
    EXPECT_TRUE(TestAngleSeek(uut, stepper, start, start + BARREL_DEG_PER_SMALL_STEP));

    //Test Negative Angle
    start = -15.0f;

    SetAngle(start);
    uut.JoyStickEventHandler(JoystickEvent::DownLarge);
    EXPECT_TRUE(TestAngleSeek(uut, stepper, start, start - BARREL_DEG_PER_LARGE_STEP));

    SetAngle(start);
    uut.JoyStickEventHandler(JoystickEvent::DownSmall);
    EXPECT_TRUE(TestAngleSeek(uut, stepper, start, start - BARREL_DEG_PER_SMALL_STEP));

    SetAngle(start);
    uut.JoyStickEventHandler(JoystickEvent::UpLarge);
    EXPECT_TRUE(TestAngleSeek(uut, stepper, start, start + BARREL_DEG_PER_LARGE_STEP));

    SetAngle(start);
    uut.JoyStickEventHandler(JoystickEvent::UpSmall);
    EXPECT_TRUE(TestAngleSeek(uut, stepper, start, start + BARREL_DEG_PER_SMALL_STEP));
}