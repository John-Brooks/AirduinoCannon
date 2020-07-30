cmake_minimum_required (VERSION 3.12.4)
project(airduinocannon)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "-O0")

add_subdirectory(googletest)

add_library(arduino_code 
    sketch/main.cpp
    sketch/FireControl.cpp
    sketch/VelocityMeasurement.cpp
    sketch/Joystick.cpp
    sketch/HomeScreen.cpp
    sketch/BarrelAngleScreen.cpp
)
include_directories(sketch)

include_directories(googletest/googletest/include)
add_executable(Tests 
    tests/TestMain.cpp
    tests/Arduino.cpp
    tests/LiquidCrystal.cpp
    tests/TestInputDebouncer.cpp
    tests/TestFireControl.cpp
    tests/TestVelocityMeasurement.cpp
    tests/TestJoystick.cpp
    tests/TestHomeScreen.cpp
    tests/TestBarrelAngleScreen.cpp
    tests/StepperMotorSim.cpp
)

target_link_libraries(Tests PUBLIC arduino_code gtest)