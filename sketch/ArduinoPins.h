#pragma once

#define FIRE_BUTTON_PIN 1
#define BARREL_1_SOLENOID_PIN 2
#define BARREL_2_SOLENOID_PIN 3
#define BARREL_3_SOLENOID_PIN 4
#define SAFETY_LIGHT_PIN 5
#define BARREL_SELECT_BUTTON_PIN 6

//macros for various states, so they can be changed easily from low-side drive
//to high side drive if necessary without touching the software
#define SOLENOID_CLOSED 0
#define SOLENOID_OPEN 1
#define SAFETY_LIGHT_OFF 0
#define SAFETY_LIGHT_ON 1