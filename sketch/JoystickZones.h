#pragma once
#include <stdint.h>

//Atmega 2560 has 10-bit analog inputs so the input range is 0-1023
//we normalize the inputs to one quadrant so all of the thresholds
//are set based on a 0-512 X and Y axis.

//use the last 10 percent of the input range for large step detection
//512 - (512*0.1) = 461
const uint16_t JOY_Y_LARGE_THRESH = 461;

//trigger small X and Y step at 33%
const uint16_t JOY_Y_SMALL_THRESH = 169;
const uint16_t JOY_X_THRESH = 169;

//set deadzone as the center 20%
const uint16_t JOY_DEADZONE = 102;

