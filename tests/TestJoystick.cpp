#include "gtest/gtest.h"
#include "Joystick.h"
#include "JoystickZones.h"
#include "Arduino.h"
#include "ArduinoPins.h"
#include "main.h"

bool event_triggered = false;
JoystickEvent last_event;

void JoystickEventHandler(JoystickEvent event)
{
    last_event = event;
    event_triggered = true;
}

void CenterJoystick()
{
    arduino.SetPinState(JOYSTICK_X_PIN, 512);
    arduino.SetPinState(JOYSTICK_Y_PIN, 512);
}

void Spin(Joystick& uut, uint32_t time_ms = 1)
{
    for(uint32_t i = 0; i < time_ms; i++)
    {
        arduino.TickMillisecond();
        uut.Run(millis());
    }
}

bool AxisApproxCentered(int axis_value, int delta)
{
    return axis_value > (512-delta) && axis_value < (512+delta);
}

int MoveTowardsCenter(int axis_value, int step)
{
    if(axis_value < 512)
        return axis_value + step;
    else   
        return axis_value - step;
}

bool ResetSpinCenterAndCheckForEvent(Joystick& uut)
{
    Spin(uut);
    event_triggered = false;
    int joy_x = arduino.GetPinState(JOYSTICK_X_PIN);
    int joy_y = arduino.GetPinState(JOYSTICK_Y_PIN);
    const int step = 5;
    const int threshold = 10;
    bool x_is_centered = AxisApproxCentered(joy_x, threshold);
    bool y_is_centered = AxisApproxCentered(joy_y, threshold);

    while(!x_is_centered || !y_is_centered)
    {
        if(!x_is_centered)
        {
            joy_x = MoveTowardsCenter(joy_x, step);
            x_is_centered = AxisApproxCentered(joy_x, threshold);
        }
        if(!y_is_centered)
        {
            joy_y = MoveTowardsCenter(joy_y, step);
            y_is_centered = AxisApproxCentered(joy_y, threshold);
        }
        arduino.SetPinState(JOYSTICK_X_PIN, joy_x);
        arduino.SetPinState(JOYSTICK_Y_PIN, joy_y);
        Spin(uut);
    }
    
    return event_triggered;
}

void InitTest(Joystick& uut)
{
    arduino.Reset();
    InitializePins();
    uut.SetEventHandler(JoystickEventHandler);

    CenterJoystick();
    Spin(uut, 500);
}

TEST(JoystickTests, ButtonPress)
{
    Joystick uut;
    InitTest(uut);

    //button depressed
    arduino.SchedulePinToggle(JOYSTICK_CLICK_PIN, 100000);
    //button released
    arduino.SchedulePinToggle(JOYSTICK_CLICK_PIN, 250000);

    for(int i = 0; i < 300; i++)
    {
        uut.Run(millis());
        arduino.TickMillisecond();
    }

    ASSERT_TRUE(event_triggered);
    ASSERT_EQ(last_event, JoystickEvent::Click);
}

TEST(JoystickTests, LeftRight)
{
    Joystick uut;
    InitTest(uut);

    //Full right
    arduino.SetPinState(JOYSTICK_X_PIN, 1023);
    ASSERT_TRUE(ResetSpinCenterAndCheckForEvent(uut));
    ASSERT_EQ(last_event, JoystickEvent::Right);

    Spin(uut, 10);

    //Full Left
    arduino.SetPinState(JOYSTICK_X_PIN, 0);
    ASSERT_TRUE(ResetSpinCenterAndCheckForEvent(uut));
    ASSERT_EQ(last_event, JoystickEvent::Left);
}

TEST(JoystickTests, UpDown)
{
    Joystick uut;
    InitTest(uut);

    //Full Up
    Spin(uut, 10);
    arduino.SetPinState(JOYSTICK_Y_PIN, 1023);
    ASSERT_TRUE(ResetSpinCenterAndCheckForEvent(uut));
    EXPECT_EQ(last_event, JoystickEvent::UpLarge);

    //Full Down
    Spin(uut, 10);
    arduino.SetPinState(JOYSTICK_Y_PIN, 0);
    ASSERT_TRUE(ResetSpinCenterAndCheckForEvent(uut));
    EXPECT_EQ(last_event, JoystickEvent::DownLarge);

    //Small Up
    Spin(uut, 10);
    arduino.SetPinState(JOYSTICK_Y_PIN, 512 + JOY_Y_SMALL_THRESH + 10);
    ASSERT_TRUE(ResetSpinCenterAndCheckForEvent(uut));
    EXPECT_EQ(last_event, JoystickEvent::UpSmall);

    //Small Down
    Spin(uut, 10);
    arduino.SetPinState(JOYSTICK_Y_PIN, (512 - JOY_Y_SMALL_THRESH) - 10);
    ASSERT_TRUE(ResetSpinCenterAndCheckForEvent(uut));
    EXPECT_EQ(last_event, JoystickEvent::DownSmall);
}

TEST(JoystickTests, Intersection)
{
    Joystick uut;
    InitTest(uut);

    //Top right corner
    Spin(uut, 10);
    arduino.SetPinState(JOYSTICK_Y_PIN, 1023);
    arduino.SetPinState(JOYSTICK_X_PIN, 1023);
    EXPECT_FALSE(ResetSpinCenterAndCheckForEvent(uut));

    //Top left corner
    Spin(uut, 10);
    arduino.SetPinState(JOYSTICK_Y_PIN, 1023);
    arduino.SetPinState(JOYSTICK_X_PIN, 0);
    EXPECT_FALSE(ResetSpinCenterAndCheckForEvent(uut));

    //bottom left corner
    Spin(uut, 10);
    arduino.SetPinState(JOYSTICK_Y_PIN, 0);
    arduino.SetPinState(JOYSTICK_X_PIN, 0);
    EXPECT_FALSE(ResetSpinCenterAndCheckForEvent(uut));

    //bottom right corner
    Spin(uut, 10);
    arduino.SetPinState(JOYSTICK_Y_PIN, 0);
    arduino.SetPinState(JOYSTICK_X_PIN, 1023);
    EXPECT_FALSE(ResetSpinCenterAndCheckForEvent(uut));
}

TEST(JoystickTests, IntersectionReturnedThroughRight)
{
    Joystick uut;
    InitTest(uut);

    //Top right corner, then dragged down to right region trigger
    //expect that a right event is fired.
    Spin(uut, 10);
    arduino.SetPinState(JOYSTICK_Y_PIN, 1023);
    arduino.SetPinState(JOYSTICK_X_PIN, 1023);
    Spin(uut, 20);
    arduino.SetPinState(JOYSTICK_Y_PIN, 512);
    ASSERT_TRUE(ResetSpinCenterAndCheckForEvent(uut));
    EXPECT_EQ(last_event, JoystickEvent::Right);
}

TEST(JoystickTests, AlmostRightNegativeTest)
{
    Joystick uut;
    InitTest(uut);

    //Top right corner, then dragged down to right region trigger
    //expect that a right event is fired.
    Spin(uut, 10);
    arduino.SetPinState(JOYSTICK_X_PIN, 512 + JOY_X_THRESH - 1);
    EXPECT_FALSE(ResetSpinCenterAndCheckForEvent(uut));
}

TEST(JoystickTests, AlmostUpNegativeTest)
{
    Joystick uut;
    InitTest(uut);

    //Top right corner, then dragged down to right region trigger
    //expect that a right event is fired.
    Spin(uut, 10);
    arduino.SetPinState(JOYSTICK_X_PIN, 512 + JOY_Y_SMALL_THRESH - 1);
    ASSERT_FALSE(ResetSpinCenterAndCheckForEvent(uut));
}