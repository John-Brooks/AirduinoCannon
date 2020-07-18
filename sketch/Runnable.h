#pragma once
#include <stdint.h>

class Runnable
{
public:
    virtual void Run(uint32_t current_time) = 0;
};
