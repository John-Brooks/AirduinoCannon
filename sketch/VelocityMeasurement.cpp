#include "VelocityMeasurement.h"
#include "Arduino.h"
#include "TimeDifference.h"

bool gate1_triggered = false;
bool gate2_triggered = false;
uint32_t gate1_time = 0;
uint32_t gate2_time = 0;
void gate1_trigger()
{
    gate1_time = micros();
    gate1_triggered = true;
}
void gate2_trigger()
{
    gate2_time = micros();
    gate2_triggered = true;
}

void VelocityMeasurement::PrepareForMeasurement()
{
    gate1_time = 0;
    gate2_time = 0;
    gate1_triggered = false;
    gate2_triggered = false;
    attachInterrupt(digitalPinToInterrupt(gate1_pin), gate1_trigger, RISING);
    attachInterrupt(digitalPinToInterrupt(gate2_pin), gate2_trigger, RISING);
}
bool VelocityMeasurement::MeasurementAvailable()
{
    return gate1_triggered && gate2_triggered;
}
float VelocityMeasurement::GetMeasurement()
{
    if(!MeasurementAvailable())
        return 0.0f;

    float time_difference = (float)GetUnsignedDifference(gate1_time, gate2_time);
    time_difference = time_difference / 1000000.0f; //convert microseconds to seconds
    float meters_per_second = gate_distance / time_difference;
    float miles_per_hour = meters_per_second * 2.23694f;
    return miles_per_hour;
}