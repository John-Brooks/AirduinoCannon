#include "gtest/gtest.h"
#include "InputDebouncer.h"

TEST(DebouncerTests, SimpleBounce)
{
    InputDebouncer uut(100);
    uint32_t current_time = 567934; //some random time
    bool debounced_value = uut.DebounceInput(false, current_time);
    ASSERT_EQ(debounced_value, false);

    //150 ms later input goes high (should be valid)
    current_time += 150;
    debounced_value = uut.DebounceInput(true, current_time);
    ASSERT_EQ(debounced_value, true);

    //25 ms later input bounces to low then high 5 ms later 
    current_time += 25;
    debounced_value = uut.DebounceInput(false, current_time);
    ASSERT_EQ(debounced_value, true);

    current_time += 5;
    debounced_value = uut.DebounceInput(true, current_time);
    ASSERT_EQ(debounced_value, true);

    //1 second later expect input to be high and debounced to be high.
    current_time += 1000;
    debounced_value = uut.DebounceInput(true, current_time);
    ASSERT_EQ(debounced_value, true);
}

TEST(DebouncerTests, Test100msDebounceTime)
{
    InputDebouncer uut(100);
    uint32_t current_time = 6513; //some random time
    bool debounced_value = uut.DebounceInput(false, current_time);
    ASSERT_EQ(debounced_value, false);

    //99 ms later input goes high (should be ignored)
    current_time += 99;
    debounced_value = uut.DebounceInput(true, current_time);
    ASSERT_EQ(debounced_value, false) << "just before debounce threshold";

    //1 ms later precisely on the edge, don't really care what the value here is tbh.
    //but validate that it always stays the same.
    current_time += 1;
    debounced_value = uut.DebounceInput(true, current_time);
    ASSERT_EQ(debounced_value, false) << "failed the exact edge time";
    
    //1 ms later just beyond the edge, should accept the new input.
    current_time += 1;
    debounced_value = uut.DebounceInput(true, current_time);
    ASSERT_EQ(debounced_value, true);
}

TEST(DebouncerTests, Test1000msDebounceTime)
{
    InputDebouncer uut(1000);
    uint32_t current_time = 6513; //some random time
    bool debounced_value = uut.DebounceInput(false, current_time);
    ASSERT_EQ(debounced_value, false);

    //99 ms later input goes high (should be ignored)
    current_time += 999;
    debounced_value = uut.DebounceInput(true, current_time);
    ASSERT_EQ(debounced_value, false) << "just before debounce threshold";

    //1 ms later precisely on the edge, don't really care what the value here is tbh.
    //but validate that it always stays the same.
    current_time += 1;
    debounced_value = uut.DebounceInput(true, current_time);
    ASSERT_EQ(debounced_value, false) << "failed the exact edge time";
    
    //1 ms later just beyond the edge, should accept the new input.
    current_time += 1;
    debounced_value = uut.DebounceInput(true, current_time);
    ASSERT_EQ(debounced_value, true);
}