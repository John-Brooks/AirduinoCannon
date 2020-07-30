#pragma once 
#include "Runnable.h"
#include "InputDebouncer.h"

enum class JoystickEvent
{
    UpSmall,
    UpLarge,
    DownSmall,
    DownLarge,
    Left,
    Right,
    Click
};

typedef void (*JoystickEventHandler_t)(JoystickEvent);

class Joystick : public Runnable
{
public:
    void SetEventHandler(JoystickEventHandler_t funct);
    void Run(uint32_t current_time);

private:
    enum class NormalizedZone
    {
        YSmall,
        YLarge,
        XSmall,
        Intersection,
        Deadzone,
        None,
    } last_zone = NormalizedZone::Deadzone;

    enum class VerticalDirection
    {
        Up,
        Down
    } last_vertical_direction = VerticalDirection::Up;
    
    enum class HorizontalDirection
    {
        Left,
        Right
    } last_horizontal_direction = HorizontalDirection::Right;

    InputDebouncer click_debouncer;
    JoystickEventHandler_t event_handler_callback;
    bool joystick_pressed = false;

    NormalizedZone GetCurrentZone();
    VerticalDirection GetCurrentVerticalDirection();
    HorizontalDirection GetCurrentHorizontalDirection();
    void HandleZoneTransition(NormalizedZone current_zone);
    void SendJoystickDirectionalEvent();
};