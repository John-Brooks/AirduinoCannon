#include "gtest/gtest.h"
#include "FireControl.h"
#include "main.h"
#include "Arduino.h"
#include "ArduinoPins.h"
#include "TimeDifference.h"

bool safety_engaged = false;
bool ReadyToFireCallback() { return !safety_engaged; }
bool ReturnTrueCallback() { return true; }
bool ReturnFalseCallback() { return false; }

void SpinFireControl(FireControl& uut, uint32_t milliseconds_to_spin)
{
    uint32_t current_time = millis();
    for(int i = 0; i < milliseconds_to_spin; i++)
    {
        uut.Loop(current_time + i);
        arduino.TickMillisecond();
    }  
}
void SpinFireControlMicroseconds(FireControl& uut, uint32_t microseconds_to_spin)
{
    uint32_t current_time = millis();
    for(int i = 0; i < microseconds_to_spin; i++)
    {
        uut.Loop(current_time + i);
        arduino.TickMicrosecond();
    }  
}
void PressButtonNormally(FireControl& uut, int button_pin)
{
    SpinFireControl(uut, 300); //ensure we're beyond any debounce threshold
    //schedule pin rising edge 100ms in future
    arduino.SchedulePinToggle(button_pin, 100000);
    //schedule pin falling edge 200ms in future
    arduino.SchedulePinToggle(button_pin, 200000);
    SpinFireControl(uut, 300);
}

TEST(FireControlTests, PinConfiguration)
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

TEST(FireControlTests, SafetyLight)
{
    arduino.Reset();
    InitializePins();

    FireControl uut;
    SpinFireControl(uut, 4567);

    uut.AddGONOGOCallback(&ReturnTrueCallback);
    uut.AddGONOGOCallback(&ReadyToFireCallback);

    //expect with the safety disengaged that the light is ON
    safety_engaged = false;
    uut.Loop(millis());
    ASSERT_EQ(arduino.GetPinState(SAFETY_LIGHT_PIN), SAFETY_LIGHT_ON);
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

TEST(FireControlTests, BarrelSelect)
{
    arduino.Reset();
    InitializePins();

    FireControl uut;;
    SpinFireControl(uut, 5231); //Move to some arbitrary time after startup

    EXPECT_EQ(uut.GetSelectedBarrel(), 1); //barrel should start at 1

    PressButtonNormally(uut, BARREL_SELECT_BUTTON_PIN);
    EXPECT_EQ(uut.GetSelectedBarrel(), 2);

    PressButtonNormally(uut, BARREL_SELECT_BUTTON_PIN);
    EXPECT_EQ(uut.GetSelectedBarrel(), 3);

    PressButtonNormally(uut, BARREL_SELECT_BUTTON_PIN);
    EXPECT_EQ(uut.GetSelectedBarrel(), 1);
}

static uint32_t valve_open_time;
static uint32_t valve_close_time;
static int valve_pin;
void valve_closed()
{
   valve_close_time = micros(); 
}
void valve_opened()
{
   valve_open_time = micros(); 
   attachInterrupt(valve_pin, (ISR)valve_closed, FALLING);
}
void reset_valve_timings()
{
    valve_open_time = 0;
    valve_close_time = 0;
}
uint32_t test_valve_time(FireControl& uut, uint32_t valve_time, int barrel)
{
    uut.SetDwellTime(barrel, valve_time); //55.27 milliseconds
    reset_valve_timings();
    attachInterrupt(valve_pin, valve_opened, RISING);
    PressButtonNormally(uut, FIRE_BUTTON_PIN);
    return GetUnsignedDifference(valve_open_time, valve_close_time);
}
TEST(FireControlTests, SolenoidTiming)
{
    //When the FireControl actually opens the valves / fires the projectile
    //it blocks everything while waiting the dwell time. So it's difficult to validate the timing
    //of the solenoids. To workaround this, I attach an "interrupt" in the simulator
    //to the solenoid control pins and that will callback to us (the unit tester)
    //and then we can validate the timings that way.
    arduino.Reset();
    InitializePins();
    valve_pin = BARREL_1_SOLENOID_PIN;
    int barrel = 0;
    FireControl uut;
    uut.AddGONOGOCallback(ReturnTrueCallback);
    SpinFireControl(uut, 30001); //Move to some arbitrary time after startup

    uint32_t valve_time = 52700; //52.7ms
    EXPECT_EQ(test_valve_time(uut, valve_time, barrel), valve_time);

    valve_time = 350; //0.350ms
    EXPECT_EQ(test_valve_time(uut, valve_time, barrel), valve_time);

    valve_time = 567145; //567.145ms
    EXPECT_EQ(test_valve_time(uut, valve_time, barrel), valve_time);
}
TEST(FireControlTests, NoFireWithSafetyOn)
{
    arduino.Reset();
    InitializePins();
    valve_pin = BARREL_1_SOLENOID_PIN;
    int barrel = 0;

    FireControl uut;
    uut.AddGONOGOCallback(ReturnFalseCallback);
    SpinFireControl(uut, 30001); //Move to some arbitrary time after startup

    uint32_t valve_time = 52700; //52.7ms
    EXPECT_EQ(test_valve_time(uut, valve_time, barrel), 0);  
    EXPECT_EQ(valve_open_time, 0);
}

TEST(FireControlTests, FireButtonStuckHighFault)
{
    //simulate electrical failure on FIRE button (holding HIGH), whereby when the safety is 
    //disengaged the system would immiedietly fire. Seems dangerous so a 
    //safety system exists to prevent this. This test verifies its functionality.

    arduino.Reset();
    InitializePins();
    valve_pin = BARREL_1_SOLENOID_PIN;
    int barrel = 0;

    reset_valve_timings();
    attachInterrupt(valve_pin, valve_opened, RISING);

    FireControl uut;
    uut.AddGONOGOCallback(ReadyToFireCallback);

    //engage the software safety
    safety_engaged = true;
    SpinFireControl(uut, 30001); //Move to some arbitrary time after startup
    
    //Activate the FIRE button fault mode where it is stuck high
    arduino.SetPinState(FIRE_BUTTON_PIN, HIGH);

    SpinFireControl(uut, 3000); //spin a few seconds
    
    //user disengaged software safety.
    safety_engaged = false;

    SpinFireControl(uut, 3000); //spin a few seconds
    EXPECT_EQ(valve_open_time, 0) << 
        "Expect that the system did not fire";
    EXPECT_TRUE(uut.HasFault());

    arduino.SetPinState(FIRE_BUTTON_PIN, LOW);
    uut.ClearFault();
    SpinFireControl(uut, 3000); //spin a few seconds


    //Attempt to fire the system.
    uint32_t valve_time = 52700; //52.7ms
    EXPECT_EQ(test_valve_time(uut, valve_time, barrel), valve_time) << 
        "Expect the system to recover from the fault mode.";  
}