#include "VelocityMeasurement.h"
#include "Arduino.h"

void VelocityMeasurement::PrepareForMeasurement()
{

}
bool VelocityMeasurement::MeasurementAvailable()
{
    return false;
}
float VelocityMeasurement::GetMeasurement()
{
    return 0.0;
}