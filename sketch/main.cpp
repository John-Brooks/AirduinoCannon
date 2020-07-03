#include "main.h"
#include "Arduino.h"
#include "ArduinoPins.h"
#include "FireControl.h"

int arduino_main()
{
    InitializePins();

    FireControl fire_control;


    while(true)
    {

    }
    return 0;
}

void InitializePins()
{
    pinMode(FIRE_BUTTON_PIN, INPUT);

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
}