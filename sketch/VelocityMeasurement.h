#pragma once

class VelocityMeasurement
{
public:
    VelocityMeasurement(int _gate1_pin, int _gate2_pin, float _gate_distance) : 
        gate1_pin(_gate1_pin), 
        gate2_pin(_gate2_pin),
        gate_distance(_gate_distance)
    {};

    void PrepareForMeasurement();
    bool MeasurementAvailable();
    float GetMeasurement();

private:
    int gate1_pin, gate2_pin;
    float gate_distance;
    bool measurement_available;
    float measurement;
};