#include "StepperMotorSim.h"
#include "Arduino.h"
#include "ArduinoPins.h"

bool pulse_happened = false;
static void PulsePinCallback()
{
    pulse_happened = true;
}

void StepperMotorSim::Run()
{
    if(!interrupt_attached)
        arduino.AttachInterrupt(STEPPER_PULS_PIN, PulsePinCallback, RISING);

    if (pulse_happened)
        pulse_happened = false;  
    else
        return;

    if(arduino.GetPinState(STEPPER_ENABLE_PIN) == LOW)
        return;

    int direction = arduino.GetPinState(STEPPER_DIR_PIN);
    if (direction == STEPPER_MOTOR_UP)
    {
        current_angle += step_size;
    }
    else
    {
        current_angle -= step_size;
    }
}