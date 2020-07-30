#include "Screen.h"

class BarrelAngleScreen : public Screen, public Runnable
{
public:
    void Draw(LiquidCrystal& lcd);
    void JoyStickEventHandler(JoystickEvent event);
    void Run(uint32_t current_time_ms);

private:
    enum class Direction
    {
        Up,
        Down
    };
    enum class State
    {
        Idle,
        Seeking,
    }; 

    float ReadBarrelAngle();
    void Pulse(Direction dir);
    void Zero();
    void Seek(uint32_t current_time_ms);
    void AdjustSeekAngle(int adjustment);
    bool SeekAngleAchieved();
    void UpdateSeekDirection();
    
    State state = State::Idle;
    Direction seeking_direction;
    float angle_seek_target;
    uint32_t last_pulse_time;
    float current_angle = 0.0;
};