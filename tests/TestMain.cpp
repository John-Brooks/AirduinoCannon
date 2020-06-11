#include "gtest/gtest.h"

extern "C"{
    #include "main.h"
}

int main()
{
    testing::InitGoogleTest();
    RUN_ALL_TESTS();
    return 0;
}

TEST(dummy_test, test1)
{
    ASSERT_EQ(arduino_main(), 0);
}