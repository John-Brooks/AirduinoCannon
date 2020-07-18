#include "Joystick.h"
#include "JoystickZones.h"
#include "ArduinoPins.h"
#include "Arduino.h"

#if defined (__linux__) || (__WIN32)
    #include <cassert>
#else
    #include <assert.h>
#endif

uint16_t NormalizeInput(uint16_t input)
{
    if(input > 512)
        return input - 512;
    else
        return 512 - input;
}

bool JoystickInLargeYZone(uint16_t y)
{
    return y > JOY_Y_LARGE_THRESH;
}
bool JoystickInSmallYZone(uint16_t y)
{
    return y > JOY_Y_SMALL_THRESH;
}
bool JoystickInXZone(uint16_t x)
{
    return x > JOY_X_THRESH;
}
bool JoystickInDeadZone(uint16_t x, uint16_t y)
{
    return x < JOY_DEADZONE && y < JOY_DEADZONE;
}

Joystick::NormalizedZone Joystick::GetCurrentZone()
{
    uint16_t raw_x = analogRead(JOYSTICK_X_PIN); 
    uint16_t raw_y = analogRead(JOYSTICK_Y_PIN);
    uint16_t normalized_x = NormalizeInput(raw_x);
    uint16_t normalized_y = NormalizeInput(raw_y);

    bool deadzone = JoystickInDeadZone(normalized_x, normalized_y);
    bool large_y = JoystickInLargeYZone(normalized_y);
    bool small_y = JoystickInSmallYZone(normalized_y);
    bool x_zone = JoystickInXZone(normalized_x);

    if(deadzone)
        return NormalizedZone::Deadzone;
    else if(small_y && x_zone)
        return NormalizedZone::Intersection;
    else if(large_y)
        return NormalizedZone::YLarge;
    else if(small_y)
        return NormalizedZone::YSmall;
    else if(x_zone)
        return NormalizedZone::XSmall;
    else
        return NormalizedZone::None;
}

void Joystick::Run(uint32_t current_time)
{
    NormalizedZone current_zone = GetCurrentZone();
    
    if(current_zone != last_zone)
        HandleZoneTransition(current_zone);

    bool joystick_button_prev = joystick_pressed;
    joystick_pressed = click_debouncer.DebounceInput(digitalRead(JOYSTICK_CLICK_PIN), current_time);
    //only fire button click event on rising edge.
    if(!joystick_button_prev && joystick_pressed)
        event_handler_callback(JoystickEvent::Click);
}

void Joystick::HandleZoneTransition(NormalizedZone current_zone)
{
    switch(current_zone)
    {
        //Transitioning TO the deadzone
        case NormalizedZone::Deadzone:
            //Where did we come FROM?
            switch(last_zone)
            {
                case NormalizedZone::None: 
                case NormalizedZone::Intersection:
                    //do nothing
                    break;
                //for all real zones trigger an event
                case NormalizedZone::XSmall:
                case NormalizedZone::YSmall:
                case NormalizedZone::YLarge:
                    SendJoystickDirectionalEvent();
                    break;
                default:
                    assert(false);
                    break;
            }
            break;
        
        //Transitioning TO one of the real event trigger zones
        case NormalizedZone::XSmall:
        case NormalizedZone::YSmall:
        case NormalizedZone::YLarge:
            //Store the horizontal and vertical directions
            last_horizontal_direction = GetCurrentHorizontalDirection();
            last_vertical_direction = GetCurrentVerticalDirection();
            break;

        case NormalizedZone::Intersection:
            //do nothing
            break;

        case NormalizedZone::None:
            //do nothing
            break;
    }

    //don't overwrite the zone we came from with None, that will
    //cause us to "forget" the zone that should be triggering the event.
    //this code kind of smells...
    if (current_zone != NormalizedZone::None &&
        !(current_zone == NormalizedZone::YSmall && last_zone == NormalizedZone::YLarge))
        last_zone = current_zone;
}
void Joystick::SendJoystickDirectionalEvent()
{
    JoystickEvent event;
    switch(last_zone)
    {
        case NormalizedZone::XSmall:
            if(last_horizontal_direction == HorizontalDirection::Left)
                event = JoystickEvent::Left;
            else
                event = JoystickEvent::Right;
            break;
        case NormalizedZone::YLarge:
            if(last_vertical_direction == VerticalDirection::Up)
                event = JoystickEvent::UpLarge;
            else
                event = JoystickEvent::DownLarge;
            break;
        case NormalizedZone::YSmall:
            if(last_vertical_direction == VerticalDirection::Up)
                event = JoystickEvent::UpSmall;
            else
                event = JoystickEvent::DownSmall;
            break;
        default:
            assert(false);
            break;
    }
    event_handler_callback(event);       
}

void Joystick::SetEventHandler(JoystickEventHandler_t funct)
{
    event_handler_callback = funct;
}

Joystick::HorizontalDirection Joystick::GetCurrentHorizontalDirection()
{
    uint16_t raw_x = analogRead(JOYSTICK_X_PIN); 
    if(raw_x > 512)
        return HorizontalDirection::Right;
    else
        return HorizontalDirection::Left;
}
Joystick::VerticalDirection Joystick::GetCurrentVerticalDirection()
{
    uint16_t raw_y = analogRead(JOYSTICK_Y_PIN); 
    if(raw_y > 512)
        return VerticalDirection::Up;
    else
        return VerticalDirection::Down;
}