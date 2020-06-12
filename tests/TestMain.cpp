#include "gtest/gtest.h"

extern "C"{
    #include "main.h"
}

int main()
{
    testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}

TEST(dummy_test, test1)
{
    ASSERT_EQ(arduino_main(), 0);
}