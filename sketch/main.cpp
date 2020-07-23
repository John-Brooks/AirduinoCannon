#include "main.h"
#include "Arduino.h"
#include "ArduinoPins.h"
#include "FireControl.h"
#include "VelocityMeasurement.h"
#include "HomeScreen.h"

VelocityMeasurement* velocity_measures[3] = {nullptr};
Screen* screens[3] = {nullptr};

bool AboutToFire(int barrel)
{
    if( barrel > 2 || barrel < 0)
        return false;
    velocity_measures[barrel]->PrepareForMeasurement(); 
    return true;  
}

int arduino_main()
{
    InitializePins();
    FireControl fire_control;

    //Setup velocity measurement.
    VelocityMeasurement barrel_1_velocity(BARREL1_GATE1_PIN, BARREL1_GATE2_PIN, 0.03);
    velocity_measures[0] = &barrel_1_velocity;
    VelocityMeasurement barrel_2_velocity(BARREL2_GATE1_PIN, BARREL2_GATE2_PIN, 0.03);
    velocity_measures[1] = &barrel_2_velocity;
    VelocityMeasurement barrel_3_velocity(BARREL3_GATE1_PIN, BARREL3_GATE2_PIN, 0.03);
    velocity_measures[2] = &barrel_3_velocity;

    //Setup fire control
    fire_control.SetAboutToFireCallback(AboutToFire);

    int focused_screen = 0;
    HomeScreen home_screen;
    screens[0] = &home_screen;


    while(true)
    {

    }
    return 0;
}

void InitializePins()
{
    pinMode(FIRE_BUTTON_PIN, INPUT);
    pinMode(BARREL_SELECT_BUTTON_PIN, INPUT);

    pinMode(BARREL_1_SOLENOID_PIN, OUTPUT);
    digitalWrite(BARREL_1_SOLENOID_PIN, SOLENOID_CLOSED);
    pinMode(BARREL_2_SOLENOID_PIN, OUTPUT);
    digitalWrite(BARREL_2_SOLENOID_PIN, SOLENOID_CLOSED);
    pinMode(BARREL_3_SOLENOID_PIN, OUTPUT);
    digitalWrite(BARREL_3_SOLENOID_PIN, SOLENOID_CLOSED);

    pinMode(SAFETY_LIGHT_PIN, OUTPUT);
    digitalWrite(SAFETY_LIGHT_PIN, SAFETY_LIGHT_OFF);

    //Barrel velocity sensor gates
    pinMode(BARREL1_GATE1_PIN, INPUT);
    pinMode(BARREL1_GATE2_PIN, INPUT);
    pinMode(BARREL2_GATE1_PIN, INPUT);
    pinMode(BARREL2_GATE2_PIN, INPUT);
    pinMode(BARREL3_GATE1_PIN, INPUT);
    pinMode(BARREL3_GATE2_PIN, INPUT);

    pinMode(JOYSTICK_X_PIN, INPUT);
    pinMode(JOYSTICK_Y_PIN, INPUT);
    pinMode(JOYSTICK_CLICK_PIN, INPUT);

    pinMode(PRESSURE_TRANSD_PIN, INPUT);
}