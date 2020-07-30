#pragma once

class StepperMotorSim
{
public:
    void Run();
    float step_size = 0.1;
    float current_angle = 0.0;

private:
    bool interrupt_attached = false;
};