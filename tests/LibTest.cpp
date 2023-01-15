#include <gtest/gtest.h>
#include "Lib.h"

TEST(TestingLib, welcome) {
    Lib lib;
    auto message = lib.welcome("User");
    ASSERT_STREQ(message, "Welcome to taste of HFT User");
}
