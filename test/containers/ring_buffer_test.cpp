#include "ring_buffer.hpp"
#include <gtest/gtest.h>

constexpr size_t large_size = 10;
constexpr size_t small_size = 3;

struct MockIMUData {
  int16_t x;
  int16_t y;
  int16_t z;
};

constexpr int16_t x = 22;
constexpr int16_t y = 10;
constexpr int16_t z = 8;

constexpr int16_t new_x = 32;

TEST(PushTest, VerifiesZeroCopy) {
  RingBuffer<MockIMUData, large_size> rb_;
  MockIMUData original_data{x, y, z};

  rb_.push(&original_data);
  EXPECT_EQ(rb_.getSize(), 1); // Ring Buffer size should be 1 now

  const MockIMUData *out_data;
  EXPECT_TRUE(
      rb_.peakTail(out_data)); // Should return true (retrieved a pointer)
  EXPECT_EQ(rb_.getSize(), 1); // Ring Buffer size should stay 1

  EXPECT_EQ(out_data, &original_data); // Output should equal input.
}

TEST(PushPopTest, VerifiesWrapping) {
  RingBuffer<MockIMUData, small_size> rb_; // [nullptr(head), nullptr, nullptr]
  EXPECT_EQ(rb_.getSize(), 0);

  MockIMUData od1{0, 1, 2};
  MockIMUData od2{3, 4, 5};
  MockIMUData od3{6, 7, 8};
  MockIMUData od4{9, 10, 11};

  rb_.push(&od1); // [od1(head), nullptr, nullptr]
  EXPECT_EQ(rb_.getSize(), 1);

  rb_.push(&od2); // [od1, od2(head), nullptr]
  EXPECT_EQ(rb_.getSize(), 2);

  rb_.push(&od3); // [od1, od2, od3(head)]
  EXPECT_EQ(rb_.getSize(), 3);

  rb_.push(&od4);              // [od4(head), od2(tail), od3]
  EXPECT_EQ(rb_.getSize(), 3); // Size should stay 3 (wraps around to index 0)

  // Now that all data is in place, we see if tail is od2 (last push should have
  // overwritten od1)
  MockIMUData *out_data;
  EXPECT_TRUE(rb_.pop(out_data));

  EXPECT_EQ(rb_.getSize(), 2); // Size should go down to 2 (popped tail)

  EXPECT_EQ(out_data, &od2);
}

TEST(PeakTest, VerifiesHeadTailPeak) {
  RingBuffer<MockIMUData, small_size> rb_;

  MockIMUData od1{0, 1, 2};
  MockIMUData od2{3, 4, 5};

  rb_.push(&od1); // [od1(head), nullptr, nullptr]
  EXPECT_EQ(rb_.getSize(), 1);

  rb_.push(&od2); // [od1, od2(head), nullptr]
  EXPECT_EQ(rb_.getSize(), 2);

  const MockIMUData *peak_head;
  EXPECT_TRUE(rb_.peakHead(peak_head));
  EXPECT_EQ(rb_.getSize(), 2);

  EXPECT_EQ(peak_head, &od2);

  const MockIMUData *peak_tail;
  EXPECT_TRUE(rb_.peakTail(peak_tail));
  EXPECT_EQ(rb_.getSize(), 2);

  EXPECT_EQ(peak_tail, &od1);
}

TEST(PopTest, VerifiesPopReturn) {
  RingBuffer<MockIMUData, small_size> rb_;
  EXPECT_EQ(rb_.getSize(), 0);

  MockIMUData od1{0, 1, 2};
  MockIMUData od2{3, 4, 5};

  MockIMUData *p1;
  EXPECT_FALSE(rb_.pop(p1));   // False because no data has been added
  EXPECT_EQ(rb_.getSize(), 0); // Size should still be zero

  // p1 should be empty
  EXPECT_EQ(p1->x, 0);
  EXPECT_EQ(p1->y, 0);
  EXPECT_EQ(p1->z, 0);

  rb_.push(&od1); // [od1(head), nullptr, nullptr]
  EXPECT_EQ(rb_.getSize(), 1);

  rb_.push(&od2); // [od1, od2(head), nullptr]
  EXPECT_EQ(rb_.getSize(), 2);

  MockIMUData *p2{};
  EXPECT_TRUE(rb_.pop(p2)); // [od1(head), nullptr, nullptr]

  MockIMUData *p3{};
  EXPECT_TRUE(rb_.pop(p3)); // [nullptr(head), nullptr, nullptr]

  MockIMUData *p4{};
  EXPECT_FALSE(rb_.pop(p4));
  EXPECT_EQ(rb_.getSize(), 0); // Size should be zero
}