#include "gtest/gtest.h"
#include "VelocityMeasurement.h"
#include "Arduino.h"
#include "ArduinoPins.h"
#include "main.h"


TEST(VelocityMeasurementTests, TestPinConfiguration)
{
    arduino.Reset();
    InitializePins();

    ASSERT_EQ(arduino.GetPinMode(BARREL1_GATE1_PIN), INPUT);
    ASSERT_EQ(arduino.GetPinMode(BARREL1_GATE2_PIN), INPUT);
    ASSERT_EQ(arduino.GetPinMode(BARREL2_GATE1_PIN), INPUT);
    ASSERT_EQ(arduino.GetPinMode(BARREL2_GATE2_PIN), INPUT);
    ASSERT_EQ(arduino.GetPinMode(BARREL3_GATE1_PIN), INPUT);
    ASSERT_EQ(arduino.GetPinMode(BARREL3_GATE2_PIN), INPUT);
}