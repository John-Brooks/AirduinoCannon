#include "main.h"
#include "Arduino.h"
#include "ArduinoPins.h"
#include "FireControl.h"
#include "VelocityMeasurement.h"
#include "HomeScreen.h"
#include "BarrelAngleScreen.h"
#include "Runnable.h"
#include "Joystick.h"

VelocityMeasurement* velocity_measures[3] = {nullptr};

const int first_screen = 0;
const int last_screen = 2;
int focused_screen = 0;
Screen* screens[3] = {nullptr};

const int NUM_RUNNABLES = 2;
Runnable* runnables[NUM_RUNNABLES] = {nullptr};

void JoystickEventFunct(JoystickEvent event)
{
    switch(event)
    {
        //Left and right changes screen so main handles these
        case JoystickEvent::Left:
            focused_screen--;
            break;
        case JoystickEvent::Right:
            focused_screen++;
            break;

            //otherwise, we forward the event to the current screen to handle.
        default:
            screens[focused_screen]->JoyStickEventHandler(event);
            break;
    }

    if(focused_screen < first_screen)
        focused_screen = first_screen;
    else if(focused_screen > last_screen)
        focused_screen = last_screen;
}

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

    HomeScreen home_screen;
    screens[0] = &home_screen;
    BarrelAngleScreen barrel_angle_screen;
    screens[1] = &barrel_angle_screen;

    LiquidCrystal lcd(LCD_RS, LCD_EN, LCD_D4, LCD_D5, LCD_D6, LCD_D7);

    Joystick joystick;
    joystick.SetEventHandler(JoystickEventFunct);

    runnables[0] = &fire_control;
    runnables[1] = &joystick;

    int last_screen_update = 0;
    while(true)
    {
        uint32_t current_time = millis();

        //Check if it's time to refresh the display (~30 Hz)
        if (current_time - last_screen_update > 33)
        {
            screens[focused_screen]->Draw(lcd);
            last_screen_update = current_time;
        }

        for(int i = 0; i < NUM_RUNNABLES; i++)
            runnables[i]->Run(millis());
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
    pinMode(ACCELEROMETER_PIN, INPUT);

    pinMode(STEPPER_PULS_PIN, OUTPUT);
    digitalWrite(STEPPER_PULS_PIN, LOW);

    pinMode(STEPPER_DIR_PIN, OUTPUT);
    digitalWrite(STEPPER_DIR_PIN, LOW);

    pinMode(STEPPER_ENABLE_PIN, OUTPUT);
    digitalWrite(STEPPER_ENABLE_PIN, HIGH);
}