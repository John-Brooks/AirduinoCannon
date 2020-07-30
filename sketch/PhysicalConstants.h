#pragma once

//meters
const float BARREL1_GATE_DISTANCE = 0.05;
const float BARREL2_GATE_DISTANCE = 0.05;
const float BARREL3_GATE_DISTANCE = 0.05;

const float AIN_MAX_VOLT = 3.3;
const int AIN_MAX_VALUE = 1023;
const float PSI_PER_VOLT = 30;

const int BARREL_DEG_PER_LARGE_STEP = 5;
const int BARREL_DEG_PER_SMALL_STEP = 1;
const int MICROS_PER_PULSE = 3;
const int MILLIS_BETWEEN_STEP = 20; //50Hz 
const float DEGREE_PER_STEP = 0.1;

const double PI = 3.141592;
const double ACCELL_BIAS = 1.5; //V
const double ACCELL_SENSITIVITY = 0.420; //V / g