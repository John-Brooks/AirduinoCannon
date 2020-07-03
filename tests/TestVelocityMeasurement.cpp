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

static uint32_t gate_1_time;
static uint32_t gate_2_time;
void gate1()
{
   gate_1_time = micros(); 
}
void gate2()
{
   gate_2_time = micros(); 
}
void reset_gate_timings()
{
    gate_1_time = 0;
    gate_2_time = 0;
}


TEST(VelocityMeasurementTests, TestMeasurement)
{
    arduino.Reset();
    InitializePins();
    //Gate distance 3 centimeters
    VelocityMeasurement uut(BARREL1_GATE1_PIN, BARREL1_GATE2_PIN, 0.03);

    //75 MPH = 33.528 m/s
    //0.03m / 33.528 m/s = 895 us.
    uut.PrepareForMeasurement();
    arduino.SchedulePinToggle(BARREL1_GATE1_PIN, 20000);
    arduino.SchedulePinToggle(BARREL1_GATE1_PIN, 20150);
    arduino.SchedulePinToggle(BARREL1_GATE2_PIN, 20895);
    arduino.SchedulePinToggle(BARREL1_GATE2_PIN, 21045);
    arduino.TickMicrosecond(30000);
    EXPECT_TRUE(uut.MeasurementAvailable());
    EXPECT_NEAR(uut.GetMeasurement(), 75.0, 0.05);

    //5 MPH = 2.2352 m/s
    //0.03m / 2.2352 m/s = 13,421 us.
    uut.PrepareForMeasurement();
    arduino.SchedulePinToggle(BARREL1_GATE1_PIN, 20000);
    arduino.SchedulePinToggle(BARREL1_GATE1_PIN, 20300);
    arduino.SchedulePinToggle(BARREL1_GATE2_PIN, 33421);
    arduino.SchedulePinToggle(BARREL1_GATE2_PIN, 33721);
    arduino.TickMicrosecond(40000);
    EXPECT_TRUE(uut.MeasurementAvailable());
    EXPECT_NEAR(uut.GetMeasurement(), 5.0, 0.05);
}

TEST(VelocityMeasurementTests, TestMissedGate1)
{
    arduino.Reset();
    InitializePins();
    //Gate distance 3 centimeters
    VelocityMeasurement uut(BARREL1_GATE1_PIN, BARREL1_GATE2_PIN, 0.03);

    //75 MPH = 33.528 m/s
    //0.03m / 33.528 m/s = 895 us.
    uut.PrepareForMeasurement();
    arduino.SchedulePinToggle(BARREL1_GATE2_PIN, 20895);
    arduino.SchedulePinToggle(BARREL1_GATE2_PIN, 21045);
    arduino.TickMicrosecond(30000);
    EXPECT_FALSE(uut.MeasurementAvailable());
    EXPECT_EQ(uut.GetMeasurement(), 0.0f);
}
TEST(VelocityMeasurementTests, TestMissedGate2)
{
    arduino.Reset();
    InitializePins();
    //Gate distance 3 centimeters
    VelocityMeasurement uut(BARREL1_GATE1_PIN, BARREL1_GATE2_PIN, 0.03);

    //75 MPH = 33.528 m/s
    //0.03m / 33.528 m/s = 895 us.
    uut.PrepareForMeasurement();
    arduino.SchedulePinToggle(BARREL1_GATE1_PIN, 20000);
    arduino.SchedulePinToggle(BARREL1_GATE1_PIN, 20300);
    arduino.TickMicrosecond(30000);
    EXPECT_FALSE(uut.MeasurementAvailable());
    EXPECT_EQ(uut.GetMeasurement(), 0.0f);
}
TEST(VelocityMeasurementTests, TestMissedBothGates)
{
    arduino.Reset();
    InitializePins();
    //Gate distance 3 centimeters
    VelocityMeasurement uut(BARREL1_GATE1_PIN, BARREL1_GATE2_PIN, 0.03);

    uut.PrepareForMeasurement();
    arduino.TickMicrosecond(30000);
    EXPECT_FALSE(uut.MeasurementAvailable());
    EXPECT_EQ(uut.GetMeasurement(), 0.0f);
}