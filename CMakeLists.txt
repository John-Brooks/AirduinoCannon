cmake_minimum_required (VERSION 3.16.3)
project(airduinocannon)

add_subdirectory(googletest)


add_library(arduino_code src/main.c)
include_directories(src)
include_directories(googletest/googletest/include)
add_executable(Tests tests/TestMain.cpp)


target_link_libraries(Tests PUBLIC arduino_code gtest)