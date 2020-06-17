#if defined(__linux__) || defined(__WIN32)
    #include "../tests/Arduino.h" //Use our implementation of the Arduino library
#else // assume Arduino
    #include <Arduino.h> //use Arduino's actual libary
#endif
