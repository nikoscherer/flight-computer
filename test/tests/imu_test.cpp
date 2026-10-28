#include "imu.hpp"

#include <gtest/gtest.h>

#include "ibus.hpp"

TEST(IMUTests, InitTest) {
    IMU imu(CommBusType::kSPI, (int*)0x01, 0x01);

    BusRequest request;
    imu.requestRead(request, 100);
}