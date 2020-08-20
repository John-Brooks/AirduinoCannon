# AirduinoCannon
[![Build Status](https://travis-ci.org/John-Brooks/AirduinoCannon.svg?branch=master)](https://travis-ci.org/John-Brooks/AirduinoCannon)

## Build Instructions

### Windows 

1. Open Git Bash...
2. `git clone [ssh url]`
3. `git submodule init`
4. `git submodule update`

From Visual Studio 2019 open AirduinoCannon folder and the CMake integration with Visual Studio will handle the rest.


### Linux

Open a terminal
1. `git clone [ssh url]`
2. `cd AirduinoCannon`
3. `git submodule init`
4. `git submodule update`
5. `mkdir build`
6. `cd build`
7. `cmake ..`
8. `make`

### Arudino

On host machine
1. `git clone [ssh url]`
2. From Arduino IDE open sketch/sketch.ino

