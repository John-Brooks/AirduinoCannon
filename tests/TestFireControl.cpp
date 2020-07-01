#include "gtest/gtest.h"
#include "FireControl.h"
#include "main.h"
#include "Arduino.h"
#include "ArduinoPins.h"

bool safety_engaged = false;
bool ReadyToFireCallback() { return !safety_engaged; }
bool ReturnTrueCallback() { return true; }
bool ReturnFalseCallback() { return false; }

TEST(FireControlTests, TestPinConfiguration)
{
    arduino.Reset();
    InitializePins();

    ASSERT_EQ(arduino.GetPinMode(FIRE_BUTTON_PIN), INPUT);
    ASSERT_EQ(arduino.GetPinMode(BARREL_1_SOLENOID_PIN), OUTPUT);
    ASSERT_EQ(arduino.GetPinMode(BARREL_2_SOLENOID_PIN), OUTPUT);
    ASSERT_EQ(arduino.GetPinMode(BARREL_3_SOLENOID_PIN), OUTPUT);
    ASSERT_EQ(arduino.GetPinMode(SAFETY_LIGHT_PIN), OUTPUT);

    ASSERT_EQ(arduino.GetPinState(BARREL_1_SOLENOID_PIN), SOLENOID_CLOSED);
    ASSERT_EQ(arduino.GetPinState(BARREL_2_SOLENOID_PIN), SOLENOID_CLOSED);
    ASSERT_EQ(arduino.GetPinState(BARREL_3_SOLENOID_PIN), SOLENOID_CLOSED);
    ASSERT_EQ(arduino.GetPinState(SAFETY_LIGHT_PIN), SAFETY_LIGHT_OFF);
}

TEST(FireControlTests, TestSafetyMechanisms)
{
    arduino.Reset();
    arduino.TickMillisecond(4567); //put us at some arbitrary point in time
    InitializePins();

    FireControl uut;

    uut.AddGONOGOCallback(&ReturnTrueCallback);
    uut.AddGONOGOCallback(&ReadyToFireCallback);

    //expect with the safety disengaged that the light is ON
    safety_engaged = false;
    uut.Loop(millis());
    ASSERT_EQ(arduino.GetPinState(SAFETY_LIGHT_PIN), SAFETY_LIGHT_ON);\
    ASSERT_EQ(arduino.GetPinState(SAFETY_LIGHT_PIN), SAFETY_LIGHT_ON);

    //expect with the safety engaged that the light is ON
    safety_engaged = true;
    arduino.TickMillisecond();
    uut.Loop(millis());
    ASSERT_EQ(arduino.GetPinState(SAFETY_LIGHT_PIN), SAFETY_LIGHT_OFF);

    //Expect that if we add a new system which is saying that it's not 
    //ready to fire that the safety light is not on.
    uut.AddGONOGOCallback(&ReturnFalseCallback);
    safety_engaged = false;
    arduino.TickMillisecond();
    uut.Loop(millis());
    ASSERT_EQ(arduino.GetPinState(SAFETY_LIGHT_PIN), SAFETY_LIGHT_OFF);
}