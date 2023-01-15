#include <gtest/gtest.h>

#include <iostream>

#include "Lib.h"

TEST(LibTests, welcome) {
  Lib lib;
  auto message = lib.welcome("Saman");
  EXPECT_EQ(message, "Welcome to taste of HFT Saman");
}
