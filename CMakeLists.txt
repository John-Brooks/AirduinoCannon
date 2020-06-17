cmake_minimum_required (VERSION 3.12.4)
project(airduinocannon)

add_subdirectory(googletest)

add_library(arduino_code 
    sketch/main.cpp
    sketch/FireControl.cpp
)
include_directories(sketch)

include_directories(googletest/googletest/include)
add_executable(Tests 
    tests/TestMain.cpp
    tests/Arduino.cpp
    tests/TestInputDebouncer.cpp
)

target_link_libraries(Tests PUBLIC arduino_code gtest)