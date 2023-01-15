#include <iostream>
#include "Lib.h"
#include <gtest/gtest.h>

TEST(LibTests, welcome )
{
    Lib lib;
    auto message = lib.welcome("Saman");
    EXPECT_EQ(message, "Welcome to taste of HFT Saman");
}
