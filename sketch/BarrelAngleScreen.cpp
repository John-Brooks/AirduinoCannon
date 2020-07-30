#include "BarrelAngleScreen.h"
#include "Arduino.h"
#include "ArduinoPins.h"
#include "PhysicalConstants.h"

void BarrelAngleScreen::Draw(LiquidCrystal& lcd)
{
    lcd.clear();
    char buffer[10];
    switch (state)
    {
        case State::Idle:
            lcd.setCursor(0,0);
            lcd.print("Barrel Angle Adj");
            break;
        case State::Seeking:
            lcd.setCursor(0,0);
            lcd.print("Target: ");
            lcd.setCursor(0,7);
            sprintf(buffer, "%3.1f deg", ReadBarrelAngle());
            lcd.print(buffer);
            break;

    }
    lcd.setCursor(0,1);
    lcd.print("Angle: ");
    lcd.setCursor(7,1);
    sprintf(buffer, "%3.1f deg", ReadBarrelAngle());
    lcd.print(buffer);
}

void BarrelAngleScreen::JoyStickEventHandler(JoystickEvent event)
{
    current_angle = ReadBarrelAngle();
    switch (event)
    {
        case JoystickEvent::Click:
            Zero();
            break;
        case JoystickEvent::DownLarge:
            AdjustSeekAngle(-BARREL_DEG_PER_LARGE_STEP);
            break;
        case JoystickEvent::DownSmall:
            AdjustSeekAngle(-BARREL_DEG_PER_SMALL_STEP);
            break;
        case JoystickEvent::UpLarge:
            AdjustSeekAngle(BARREL_DEG_PER_LARGE_STEP);
            break;
        case JoystickEvent::UpSmall:
            AdjustSeekAngle(BARREL_DEG_PER_SMALL_STEP);
            break;
        default:
            break;
    }
}

void BarrelAngleScreen::AdjustSeekAngle(int adjustment)
{
    if(state == State::Idle)
        angle_seek_target = current_angle;

    angle_seek_target += adjustment;
    UpdateSeekDirection();
    state = State::Seeking;
}

double ConvertAINtoAngle(int value)
{
    double voltage = ((double)value / AIN_MAX_VALUE) * AIN_MAX_VOLT;
    double gravities = (voltage - ACCELL_BIAS) / ACCELL_SENSITIVITY;

    //constrain the gravitiy value to the valid domain of arcsin
    if(gravities < -1)
        gravities = -1;
    else if(gravities > 1)
        gravities = 1;

    double radians = asin(gravities);
    double angle = (radians / (2 * M_PI)) * 360;
    return angle;   
}

float BarrelAngleScreen::ReadBarrelAngle()
{
    return ConvertAINtoAngle(analogRead(ACCELEROMETER_PIN));
}
void BarrelAngleScreen::Run(uint32_t current_time_ms)
{
    switch(state)
    {
        case State::Idle:
            return;
        case State::Seeking:
            Seek(current_time_ms);
            break;
        default:
            break;
    }
}

bool BarrelAngleScreen::SeekAngleAchieved()
{
    current_angle = ReadBarrelAngle();

    switch(seeking_direction)
    {
        case Direction::Down:
            if(current_angle <= angle_seek_target)
                return true;
            else
                return false;
        case Direction::Up:
            if(current_angle >= angle_seek_target)
                return true;
            else
                return false;
        default:
            return false;
    }
    return false;
}

void BarrelAngleScreen::Seek(uint32_t current_time_ms)
{
    if (current_time_ms - last_pulse_time < MILLIS_BETWEEN_STEP)
        return;

    if (SeekAngleAchieved())
    {
        state = State::Idle;
    }
    else
    {
        Pulse(seeking_direction);
    }
}
void BarrelAngleScreen::Pulse(Direction dir)
{
    digitalWrite(STEPPER_ENABLE_PIN, HIGH);
    
    if (dir == Direction::Up)
        digitalWrite(STEPPER_DIR_PIN, STEPPER_MOTOR_UP);
    else
        digitalWrite(STEPPER_DIR_PIN, STEPPER_MOTOR_DOWN);
    
    digitalWrite(STEPPER_PULS_PIN, HIGH);
    delayMicroseconds(MICROS_PER_PULSE);
    digitalWrite(STEPPER_PULS_PIN, LOW);
}

void BarrelAngleScreen::UpdateSeekDirection()
{
    if(current_angle < angle_seek_target)
        seeking_direction = Direction::Up;
    else
        seeking_direction = Direction::Down;
}

void BarrelAngleScreen::Zero()
{
    angle_seek_target = 0.0;
    state = State::Seeking;
    UpdateSeekDirection();
}
