#if defined(__linux__) || defined(__WIN32)
    #include "../tests/LiquidCrystal.h" //Use our implementation of the Arduino library
#else // assume Arduino
    #include <LiquidCrystal.h> //use Arduino's actual libary
#endif
