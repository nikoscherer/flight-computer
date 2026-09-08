#include <gtest/gtest.h>

// A simple function to test
int add(int a, int b) { return a + b; }

// Test positive numbers
TEST(AdditionTest, HandlesPositiveNumbers) {
  EXPECT_EQ(add(2, 3), 5);
  EXPECT_EQ(add(10, 20), 30);
}

// Test negative numbers and zero
TEST(AdditionTest, HandlesNegativeAndZero) {
  EXPECT_EQ(add(-2, -3), -5);
  EXPECT_EQ(add(5, -5), 0);
}