#pragma once

//This function only works for unsigned types, but provides rollover protection for measuring
//time deltas.
template<typename T>
T GetUnsignedDifference(T& start, T& end)
{
    //trivial case
    if(start <= end)
        return end - start;

    //rollover case
    T max_value = (T)-1;
    return start + (max_value - end);
}
